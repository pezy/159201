#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace ads{

struct Node
{
    int row     ;
    int column  ;
    int value   ;
    Node *next  ;
};

inline std::ostream&
operator<<(std::ostream& os, Node const& rhs)
{
    return os << rhs.row << " " << rhs.column << " " << rhs.value << std::endl;
}

template<typename Node>
class SparseMatrix
{
public:
    using ValueType =   decltype(Node::value);
    using IndexType =   decltype(Node::row);
    using SizeType  =   std::size_t;

    SparseMatrix() = delete;

    /**
     * @brief Ctor using file name
     * @param fn
     *
     * @abstraction Top
     */
    explicit SparseMatrix(std::string fn):
        head_{nullptr},
        tail_{nullptr},
        rows_{0},
        cols_{0}
    {
        construct_linked_list(fn);
    }

    /**
     * @brief empty
     *
     * @abstraction Top
     */
    bool empty()const
    {
        return !head_ and !tail_;
    }

    /**
     * @brief size
     *
     * @abstraction Top
     */
    SizeType data_size()const
    {
        SizeType len = 0;
        for(auto curr = head_; curr; curr = curr->next) ++len;
        return len;
    }

private:
    Node* head_;
    Node* tail_;
    SizeType rows_;
    SizeType cols_;

    /**
     * @brief add
     * @param node
     *
     * @abstraction III
     */
    void add(Node && node)
    {
        if(empty()){
            head_ = tail_ = new Node(std::move(node));
        }
        else{
            tail_->next =   new Node(std::move(node));
            tail_       =   tail_->next;
        }

        std::cout << "ads> added:" << *tail_ << std::endl;
    }

    /**
     * @brief read_and_init_dimensions
     * @param ifs
     * @return ifs
     *
     * @abstraction II
     */
    std::ifstream& read_and_init_dimensions(std::ifstream& ifs)
    {
        std::string line;
        std::getline(ifs, line);
        std::stringstream stream{line};
        stream >> rows_ >> cols_;
        std::cout << "Matrix dimensions " << rows_ << " " << cols_ << std::endl;
        return ifs;
    }

    /**
     * @brief read_and_init_matrix_body
     * @param ifs
     * @return ifs
     *
     * @abstraction II
     */
    std::ifstream& read_and_init_body(std::ifstream& ifs)
    {
        std::string line;
        for(IndexType r=0; r != rows_; ++r){
            std::getline(ifs,line);
            std::stringstream stream{line};
            for(IndexType c=0; c != cols_; ++c){
                ValueType value{0};
                stream >> value;
                if(value)
                    add({r, c, value, nullptr});
            }
        }
        return ifs;
    }

    /**
     * @brief construct_linked_list
     * @param fn
     * @abstraction I
     */
    void construct_linked_list(std::string const& fn)
    {
        std::ifstream ifs{fn};
        if(!ifs.good())
            throw std::runtime_error{"Cannot open file " + fn};
        read_and_init_body(read_and_init_dimensions(ifs));
    }
};


}//namespace

int main()
{
    ads::SparseMatrix<ads::Node> m{"matrix1.txt"};
    std::cout << "size : " <<m.data_size() << std::endl;

    return 0;
}


//! output:
//Matrix dimensions 4 4
//ads> added:0 1 1

//ads> added:1 2 2

//ads> added:2 1 3

//ads> added:2 3 4

//ads> added:3 3 5

//size : 5