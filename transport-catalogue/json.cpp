#include "json.h"

using namespace std;

namespace json {

    namespace {

        using Number = std::variant<int, double>;

        Node LoadNode(istream& input);

        Node LoadNull(istream& input) {
            const string nameNull = "null";
            for (size_t i = 0; i < nameNull.size(); i++) {
                if (nameNull.at(i) == input.get()) continue;
                else throw ParsingError("Null parsing error");
            }

            if (isalpha(input.peek())) {
                throw ParsingError("Null parsing error");
            }

            return {};
        }

        
        std::string LoadString(std::istream& input) {
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                   
                    ++it;
                    if (it == end) {
                        
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    
                    switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r') {
                    
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    
                    s.push_back(ch);
                }
                ++it;
            }

            return s;
        }

        Node LoadNumber(std::istream& input) {
            using namespace std::literals;

            std::string parsed_num;

            
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
                };

            
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
                };

            if (input.peek() == '-') {
                read_char();
            }
            
            if (input.peek() == '0') {
                read_char();
                
            }
            else {
                read_digits();
            }

            bool is_int = true;
           
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    
                    try {
                        return std::stoi(parsed_num);
                    }
                    catch (...) {
                        
                    }
                }
                return std::stod(parsed_num);
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        Node LoadBool(istream& input) {
            bool value = (input.peek() == 't');
            std::string const name = value ? "true" : "false";

            for (size_t i = 0; i < name.size(); i++) {
                if (name.at(i) != input.get()) throw ParsingError("Bool parsing error");
            }

            if (isalpha(input.peek())) {
                throw ParsingError("Bool parsing error");
            }

            return Node(value);
        }

        Node LoadArray(istream& input) {
            Array result;
            if (input.peek() == -1) throw ParsingError("Array parsing error");

            for (char c; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }
            return Node(std::move(result));
        }

        Node LoadDict(istream& input) {
            Dict result;
            if (input.peek() == -1) throw ParsingError("Array parsing error");

            for (char c; input >> c && c != '}';) {
                if (c == ',') {
                    input >> c;
                }

                string key = LoadString(input);
                input >> c;
                result.insert({ std::move(key), LoadNode(input) });
            }

            return Node(std::move(result));
        }

        Node LoadNode(istream& input) {
            char c;
            input >> c;

            if (c == 'n') {
                input.putback(c);
                return LoadNull(input);
            }
            else if (c == '"') {
                return LoadString(input);
            }
            else if (c == 't' || c == 'f') {
                input.putback(c);
                return LoadBool(input);
            }
            else if (c == '[') {
                return LoadArray(input);
            }
            else if (c == '{') {
                return LoadDict(input);
            }
            else {
                input.putback(c);
                return LoadNumber(input);
            }
        }

    }  // namespace

    bool Node::IsInt() const {
        return holds_alternative<int>(*this);
    }

    bool Node::IsDouble() const {
        return holds_alternative<double>(*this) || holds_alternative<int>(*this);
    }

    bool Node::IsPureDouble() const {
        return holds_alternative<double>(*this);
    }

    bool Node::IsBool() const {
        return holds_alternative<bool>(*this);
    }

    bool Node::IsString() const {
        return holds_alternative<std::string>(*this);
    }

    bool Node::IsNull() const {
        return holds_alternative<std::nullptr_t>(*this);
    }

    bool Node::IsArray() const {
        return holds_alternative<Array>(*this);
    }

    bool Node::IsMap() const {
        return holds_alternative<Dict>(*this);
    }

    int Node::AsInt() const {
        if (!IsInt()) throw std::logic_error("wrong type");
        return std::get<int>(*this);
    }

    bool Node::AsBool() const {
        if (!IsBool()) throw std::logic_error("wrong type");
        return std::get<bool>(*this);
    }

    double Node::AsDouble() const {
        if (!IsDouble()) throw std::logic_error("wrong type");
        if (IsInt()) return static_cast<double>(std::get<int>(*this));
        return std::get<double>(*this);
    }

    const std::string& Node::AsString() const {
        if (!IsString()) throw std::logic_error("wrong type");
        return std::get<std::string>(*this);
    }

    const Array& Node::AsArray() const {
        if (!IsArray()) throw std::logic_error("wrong type");
        return std::get<Array>(*this);
    }

    const Dict& Node::AsMap() const {
        if (!IsMap()) throw std::logic_error("wrong type");
        return std::get<Dict>(*this);
    }

    const Value& Node::GetValue() const {
        return *this;
    }

    bool Node::operator==(const Node& rhs) const {
        return *this == rhs.GetValue();
    }

    bool Node::operator!=(const Node& rhs) const {
        return !(*this == rhs.GetValue());
    }

    Document::Document(Node root) : root_(std::move(root)) { }

    const Node& Document::GetRoot() const {
        return root_;
    }

    bool Document::operator==(const Document& rhs) const {
        return root_ == rhs.root_;
    }

    bool Document::operator!=(const Document& rhs) const {
        return !(root_ == rhs.root_);
    }

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }

    void ValuePrinter::operator()(std::nullptr_t) {
        out << "null"sv;
    }

    void ValuePrinter::operator()(std::string value) {
        out << "\""sv;
        for (const char& c : value) {
            if (c == '\n') {
                out << "\\n"sv;
                continue;
            }
            if (c == '\r') {
                out << "\\r"sv;
                continue;
            }
            if (c == '\"') {
                out << "\\"sv;
            }
            if (c == '\t') {
                out << "\\t"sv;
                continue;
            }
            if (c == '\\') {
                out << "\\"sv;
            }
            out << c;
        }
        out << "\""sv;
    }

    void ValuePrinter::operator()(int value) {
        out << value;
    }

    void ValuePrinter::operator()(double value) {
        out << value;
    }

    void ValuePrinter::operator()(bool value) {
        out << std::boolalpha << value;
    }

    void ValuePrinter::operator()(Array array) {
        out << "["sv;
        bool first = true;
        for (const auto& elem : array) {
            if (first) first = false;
            else out << ", "s;

            std::visit(ValuePrinter{ out }, elem.GetValue());
        }
        out << "]"sv;
    }

    void ValuePrinter::operator()(Dict dict) {
        out << "{ "sv;
        bool first = true;
        for (auto& [key, node] : dict) {
            if (first) first = false;
            else out << ", "s;
            out << "\"" << key << "\": ";
            std::visit(ValuePrinter{ out }, node.GetValue());
        }
        out << " }"sv;
    }

    void Print(const Document& doc, std::ostream& out) {
        std::visit(ValuePrinter{ out }, doc.GetRoot().GetValue());
    }

}  // namespace json
