#include <iostream>
#include <fstream>
#include <locale>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>


namespace ads {

/**
 * @brief The Queue class
 */
template<typename T>
class Queue
{
    struct Node
    {
        T data_;
        Node* next_;
    };

public:
    using SizeType = std::size_t;

    void join(T t)
    {
        do_join(std::move(t));
    }

    void leave()
    {
        do_leave();
    }

    T const& front() const
    {
        return head_->data_;
    }

    SizeType size()const
    {
        return size_;
    }

    bool empty()const
    {
        return !head_ and !tail_ ;
    }

private:
    Node* head_{nullptr};
    Node* tail_{nullptr};
    SizeType size_{0};

    void do_join(T&& t)
    {
        if(empty())
            head_ = tail_        = new Node{std::move(t), nullptr};
        else
            tail_ = tail_->next_ = new Node{std::move(t), nullptr};
        ++size_;
    }

    void do_leave()
    {
        if(size_ == 0)
            throw std::runtime_error{"underflow!"};

        if(size_ == 1)
        {
            delete head_;
            head_ = tail_ = nullptr;
        }
        else
        {
            auto tmp = head_;
            head_ = head_->next_;
            delete tmp;
        }
        --size_;
    }
};


/**
 * @brief The FileReader class
 *
 * RAII for ifstream
 */
class FileReader
{
public:
    FileReader(std::string fn): ifs_{fn}
    {
        if(!ifs_.good())
            throw std::runtime_error{"can not open file"};
    }

    bool is_end() const
    {
        return ifs_.eof();
    }

    std::string read()
    {
        std::string line;
        std::getline(ifs_, line);
        return line;
    }

private:
    std::ifstream ifs_;
};


/**
 * @brief The Pool struct
 */
template<typename T>
struct Pool : public std::vector<ads::Queue<T> >
{
    using Super = std::vector<ads::Queue<T> >;
    void pop_each() //for output queue
    {
        for(auto& q : *this) q.leave();
    }

    T total_size()const
    {
        T ret = 0;
        for(auto const& q : *this) ret += q.size();
        return ret;
    }
};

template<typename T>
struct Congestion : public std::vector<T>
{
    typename std::vector<T>::size_type
    total_size()const
    {
        return std::accumulate(this->cbegin(), this->cend(), 0);
    }
};

class Simulator
{
public:
    explicit Simulator(std::string const& fn): rx_{}, tx_{}, congestion_{}
    {
        do_constructor(fn);
    }

    void run()
    {
        do_run();
    }

private:
    Pool<int> rx_;
    Pool<int> tx_;
    Congestion<int> congestion_;

    //! level I
    void do_constructor(std::string const& fn)
    {
        std::size_t port = 0;
        for(FileReader file{fn}; ! file.is_end(); )
        {
            std::string line = file.read();

            if(line[0] == '#')
                continue;

            if(line[0] == 'P')
            {
                std::stringstream ss{line};
                std::string token;
                std::getline(ss, token, ' ');
                std::getline(ss, token, ' ');
                int num_of_ports = std::stoi(token);

                rx_.resize(num_of_ports);
                tx_.resize(num_of_ports);
                congestion_.resize(num_of_ports, 0);
                continue;
            }

            if(std::isdigit(line[0]))
            {
                ++port;
                std::stringstream ss{line};
                for(std::string token; std::getline(ss, token, ' '); )
                    rx_[port].join(std::stoi(token));
                continue;
            }
        }
    }

    //! level I
    void do_run()
    {
        std::size_t clock = 0;
        for(std::size_t port = 0; tx_.total_size() > 0; ++port)
        {
            //! rx to tx
            port %= tx_.size();
            if(! rx_[port].empty())
            {
                auto dest = rx_[port].front();
                rx_[port].leave();
                tx_[dest].join(dest);
            }

            //! delete from tx
            ++clock;
            if(is_time_to_delete(clock))
            {
                tx_.pop_each();
            }

            ////
            ////    working here
            ////
        }
    }

    //! level II
    template<typename Clock>
    bool is_time_to_delete(Clock const& clock) const
    {
        return clock % (3 * tx_.size()) == 0 and clock != 0;
    }
};


}//namespace

int main()
{
    ads::Simulator sim{"simulation1.txt"};
    return 0;
}
