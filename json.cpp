#include "json.hpp"
using namespace std;
//MODIFICHE DAL 4o REPORT : CHIAVE BUG FIXATA (ANCHE SE FUNZIONAVA LO STESSO), TOLTO "ESTENSIONE" ITERATOR DI VECTOR E TOLTO SPACE COME VARIABILE GLOBALE E TOLTO DALLE FUNZIONI PRINT LIST E PRINT DICTIONARY + checkValidStream
template<typename T>
class Vector{
public:
    struct Cell{
        Cell* next;
        T info;
        Cell(){
            this->next = nullptr;
        }
        Cell(const T& info){
            this->info = info;
            this->next = nullptr;
        }
    };
    typedef Cell* Pcell;
    Pcell headJson;
    Pcell tailJson;
    Vector(){
        this->headJson = this->tailJson = nullptr;
    }
    Vector(const Vector<T>& v){
        this->headJson = this->tailJson = nullptr;
        Pcell source = v.headJson;
        Pcell dest = nullptr;
        while(source != nullptr){
            auto newNode = new Cell(source->info);
            if(!dest){
                this->headJson = newNode;
            }
            else{
                dest->next = newNode;
            }
            dest = newNode;
            source = source->next;
        }
        tailJson = dest;
    }
    Vector(Vector<T>&& v) noexcept{
        this->headJson = std::move(v.headJson);
        this->tailJson = std::move(v.tailJson);
        v.headJson = nullptr;
        v.tailJson = nullptr;
    }
    Vector<T>& operator=(const Vector<T>& v){
        if (this != &v) {
            this->clearVector();
            headJson = tailJson = nullptr;
            *this = Vector<T>(v); 
        }
        return *this;
    }
    Vector<T>& operator=(Vector<T>&& v) noexcept{
        if (this != &v) {
            this->clearVector();
            headJson = std::move(v.headJson);
            tailJson = std::move(v.tailJson);
            v.headJson = v.tailJson = nullptr;
        }
        return *this;
    }
    ~Vector(){
        this->clearVector();
    }
    bool empty(){
        return this->headJson == nullptr;
    }
    void toStringJson(){
        Vector<T>::Iterator print = this->headJson;
        while(print != nullptr){
            std::cout << *print << " ";
            ++print;
        }
        std::cout << std::endl;
    }
    void clearVector(){
        Pcell pc = this->headJson;
        while (pc != nullptr) {
            Pcell temp = pc;
            pc = pc->next;
            delete temp;
        }
        this->headJson = this->tailJson = nullptr;
    }
    void pushBack(const T& value){
        auto newNode = new Cell(value);
        if(this->empty()){
            this->headJson = newNode;
            this->tailJson = newNode;  
        } 
        else{
            this->tailJson->next = newNode;
            this->tailJson = newNode;  
        } 
    }
    void prepend(const T& value){
        auto newNode = new Cell(value);
        if(this->empty()){
            this->headJson = newNode;
            this->tailJson = newNode;
        }
        else{
            newNode->next = this->headJson;
            this->headJson = newNode;
        }
    }
    //--------------------------ITERATOR----------------------
    class Iterator{
    public:
        Pcell current;
        Iterator(Pcell current) : current(current) {}
        Iterator& operator++(){
            current = current->next;
            return *this;
        }
        Iterator operator++(int){
            Iterator it = *this;
            ++(*this);
            return it;
        }
        bool operator==(const Iterator& other) const{
            return current == other.current;
        }
        bool operator!=(const Iterator& other) const{
            return !(*this == other);
        }
        T& operator*() const{
            return current->info;
        }
        T* operator->() const{
            return &(current->info);
        }
    };
    Iterator beginJson(){
        return this->headJson;
    }
    Iterator endJson(){
        return nullptr;
    }
    Iterator getTail(){
        return this->tailJson;
    }
};

struct json::impl{
    Vector<pair<string, json>> Dizionario;
    Vector<json> Lista;
    impl() {
        tipo = "Nullo";
        boolean = false;
        number = 0.0;
        str = "";
    }
    impl(const impl& other){
        tipo = other.tipo;
        boolean = other.boolean;
        number = other.number;
        str = other.str;
        Lista = other.Lista;
        Dizionario = other.Dizionario;
    }
    impl(impl&& other){
        tipo = std::move(other.tipo);
        boolean = other.boolean;
        number = other.number;
        str = std::move(other.str);
        Lista = std::move(other.Lista);
        Dizionario = std::move(other.Dizionario);
    }
    impl& operator=(const impl& other){
        if (this != &other) {
            this->Lista = other.Lista;
            this->Dizionario = other.Dizionario;
            this->tipo = other.tipo;
            this->boolean = other.boolean;
            this->number = other.number;
            this->str = other.str;
        }
        return *this;
    }
    impl& operator=(impl&& other) noexcept {
        if (this != &other) {
            this->tipo = std::move(other.tipo);
            this->str = std::move(other.str);
            this->Lista = std::move(other.Lista);
            this->Dizionario = std::move(other.Dizionario);
            this->boolean = other.boolean;
            this->number = other.number;
        }
        return *this;
    }
    ~impl(){
        if(this->tipo == "List"){
            this->Lista.clearVector();
        }
        else if(this->tipo == "Dizio"){
            this->Dizionario.clearVector();
        }
    }
    string tipo;
    bool boolean;
    double number;
    string str;
    Vector<json> getList(){
        return this->Lista;
    }
    Vector<pair<string, json>> getDizio(){
        return this->Dizionario;
    }
};

json::json(){
    this->pimpl = new impl();
}

json::json(json&& j) : json(){
    *this = std::move(j);
}

json::json(const json &other){
    this->pimpl = new impl(*other.pimpl);
}

json &json::operator=(json const &j){
    if (this == &j) return *this;
    delete this->pimpl;
    if(j.pimpl != nullptr){
        this->pimpl = new impl(*j.pimpl);
    }
    else{
        this->pimpl = nullptr;
    }
    return *this;
}
json& json::operator=(json&& j){
    if (this == &j) return *this;
    delete pimpl;
    pimpl = j.pimpl;
    j.pimpl = new impl(); 
    return *this;
}

json::~json(){
    delete this->pimpl;
}

struct json::dictionary_iterator{
    using pointer = pair<string, json>*;
    using reference = pair<string, json>&;
    dictionary_iterator(Vector<pair<string, json>>::Iterator pc) : it(pc) {}
    reference operator*() const { return it.operator*(); }
    pointer operator->() const { return it.operator->(); }
    dictionary_iterator operator++() {
        ++it;
        return *this;
    }
    dictionary_iterator operator++(int) {
        dictionary_iterator it2 = {it};
        ++(*this);
        return it2;
    }
    operator bool() const { return it != nullptr; }
    bool operator==(dictionary_iterator const& other) const { return it == other.it; }
    bool operator!=(dictionary_iterator const& other) const { return it != other.it; }
    Vector<pair<string, json>>::Iterator it;
};

struct json::const_dictionary_iterator{
    using pointer = pair<string, json>*;
    using reference = pair<string, json>&;
    const_dictionary_iterator(Vector<pair<string, json>>::Iterator pc) : it(pc) {}
    reference operator*() const { return it.operator*(); }
    pointer operator->() const { return it.operator->(); }
    const_dictionary_iterator operator++() {
        ++it;
        return *this;
    }
    const_dictionary_iterator operator++(int) {
        const_dictionary_iterator it2 = {it};
        ++(*this);
        return it2;
    }
    operator bool() const { return it != nullptr; }
    bool operator==(const_dictionary_iterator const& other) const { return it == other.it; }
    bool operator!=(const_dictionary_iterator const& other) const { return it != other.it; }
    Vector<pair<string, json>>::Iterator it;
};

json::dictionary_iterator json::begin_dictionary(){
    if (!is_dictionary()) throw json_exception{"Non è dizio"};
    else return pimpl->Dizionario.beginJson();
}

json::dictionary_iterator json::end_dictionary(){
    if (!is_dictionary()) throw json_exception{"Non è dizio"};
    else return pimpl->Dizionario.endJson();
}

json::const_dictionary_iterator json::begin_dictionary() const{
    if (!is_dictionary()) throw json_exception{"Non è dizio Begin"};
    else return pimpl->Dizionario.beginJson();
}

json::const_dictionary_iterator json::end_dictionary() const{
    if (!is_dictionary()) throw json_exception{"Non è dizio End"};
    else return pimpl->Dizionario.endJson();
}

struct json::list_iterator{
    //using value_type = json;
    using pointer = json*;
    using reference = json&;
    list_iterator(Vector<json>::Iterator pc) : it(pc) {}
    reference operator*() const { return it.operator*(); }
    pointer operator->() const { return it.operator->(); }
    list_iterator& operator++() {
        ++it;
        return *this;
    }
    list_iterator operator++(int) {
        list_iterator it2 = {it};
        ++(*this);
        return it2;
    }
    operator bool() const { return it != nullptr; }
    bool operator==(list_iterator const& other) const { return it == other.it; }
    bool operator!=(list_iterator const& other) const { return it != other.it; }
    Vector<json>::Iterator it;
};
struct json::const_list_iterator{
    //using value_type = json const;
    using pointer = json const*;
    using reference = json const&;
    const_list_iterator(Vector<json>::Iterator const pc) : it(pc) {}
    reference operator*() const { return it.operator*(); }
    pointer operator->() const { return it.operator->(); }
    const const_list_iterator& operator++() {
        ++it;
        return *this;
    }
    const const_list_iterator operator++(int) {
        const_list_iterator it2 = {it};
        ++(*this);
        return it2;
    }
    operator bool() const { return it != nullptr; }
    bool operator==(const_list_iterator const& other) const { return it == other.it; }
    bool operator!=(const_list_iterator const& other) const { return it != other.it; }
    Vector<json>::Iterator it;
};

json::list_iterator json::begin_list(){
    if (!is_list()) throw json_exception{"Non è lista"};
    else return pimpl->Lista.beginJson();
}

json::list_iterator json::end_list(){
    if (!is_list()) throw json_exception{"Non è lista"};
    else return pimpl->Lista.endJson();
}

json::const_list_iterator json::begin_list() const{
    if (!is_list()) throw json_exception{"Non è lista"};
    else return pimpl->Lista.beginJson();
}

json::const_list_iterator json::end_list() const{
    if (!is_list()) throw json_exception{"Non è lista"};
    else return pimpl->Lista.endJson();
}

void json::push_back(json const& info){
    if(is_list()) this->pimpl->Lista.pushBack(info);
    else throw json_exception{"Non è una lista"};
}
void json::push_front(json const& info){
    if(is_list()) this->pimpl->Lista.prepend(info);
    else throw json_exception{"Non è una lista"};
}
void json::insert(pair<string,json> const& info){
    if (is_dictionary()) this->pimpl->Dizionario.pushBack(info);
    else throw json_exception{"Non è una dizio Insert"};
}

void printList(ostream& lhs, json const& rhs){
    json::const_list_iterator it = rhs.begin_list();
    while (it != rhs.end_list()){
        lhs << it.operator*();
        if(++it != rhs.end_list()){
            lhs << ',';
        }
    }
}

void printDictionary(ostream& lhs, json const& rhs){
    json::const_dictionary_iterator it = rhs.begin_dictionary();
    while (it != rhs.end_dictionary()){
        lhs << it.operator*().first << ':' << it.operator*().second;
        if(++it != rhs.end_dictionary()){
            lhs << ',';
        }
    }
}

ostream& operator<<(ostream& lhs, json const& rhs){
    if (rhs.is_null()) {
        return lhs << "\033[1;31mnull\033[0m";
    }
    else if (rhs.is_bool()) {
        return lhs << (rhs.get_bool() ? "\033[1;32mtrue\033[0m" : "\033[1;32mfalse\033[0m");
    }
    else if (rhs.is_number()) {
        return lhs << "\033[1;36m" << scientific << fixed << rhs.get_number() << "\033[0m";
    }
    else if (rhs.is_string()) {
        return lhs << "\033[1;33m\"" << rhs.get_string() << "\"\033[0m";
    }
    else if (rhs.is_list()) {
        lhs << '[';
        printList(lhs, rhs);
        lhs << ']';
        return lhs;
    }
    else if (rhs.is_dictionary()) {
        lhs << '{';
        printDictionary(lhs, rhs);
        lhs << '}';
        return lhs;
    }
    else {
        throw json_exception{"errore nella stampa"};
    }
}

//-------------------------------------------Functions BOOL IS----------------------------------------------------------
bool json::is_list() const{
    return pimpl->tipo == "List";
}
bool json::is_dictionary() const {
    return pimpl->tipo == "Dizio";
}
bool json::is_string() const {
    return pimpl->tipo == "String";
}
bool json::is_number() const {
    return pimpl->tipo == "Number";
}
bool json::is_bool() const {
    return pimpl->tipo == "Bool";
}
bool json::is_null() const {
    return pimpl->tipo == "Nullo";
}
//-------------------------------------------Functions GET--------------------------------------------------------------
double& json::get_number(){
    if(is_number()) return pimpl->number;
    else throw json_exception{"Non è number"};
}
double const& json::get_number() const{
    if(is_number()  && pimpl!= nullptr) return pimpl->number;
    else throw json_exception{"Non è number || pimpl == nullptr"};
}
bool& json::get_bool(){
    if(is_bool()) return pimpl->boolean;
    else throw json_exception{"Non è booleano"};
}
bool const& json::get_bool() const{
    if(is_bool()  && pimpl!= nullptr) return pimpl->boolean;
    else throw json_exception{"Non è booleano || pimpl == nullptr"};
}
string& json::get_string(){
    if(is_string()) return pimpl->str;
    else throw json_exception{"Non è una string"};
}
string const& json::get_string() const{
    if(is_string() && pimpl!= nullptr) return pimpl->str;
    else throw json_exception{"Non è una string || pimpl == nullptr"};
}

//-------------------------------------------Functions Set--------------------------------------------------------------
void json::set_bool(bool boolean) {
    delete pimpl;
    pimpl = new impl();
    pimpl->tipo = "Bool";
    pimpl->boolean = boolean;
}

void json::set_null(){
    delete pimpl;
    pimpl = new impl();
    pimpl->tipo = "Nullo";
}

void json::set_number(double number){
    delete pimpl;
    pimpl = new impl();
    pimpl->tipo = "Number";
    pimpl->number = number;
}

void json::set_list(){
    delete pimpl;
    pimpl = new impl();
    pimpl->tipo = "List";
}

void json::set_dictionary(){
    delete pimpl;
    pimpl = new impl();
    pimpl->tipo = "Dizio";
}

void json::set_string(const string &str){
    delete pimpl;
    pimpl = new impl();
    pimpl->tipo = "String";
    pimpl->str = str;
}

void skip(istream& lhs, char& current){
    while (lhs >> current and (isspace(current) or current == '\n' or current == '\r' or current == '\t'));
}

bool checkValidStream(istream& lhs){ //il controllo lhs.good() già include la verifica di not lhs.fail() e not lhs.bad(), poiché good() restituisce true solo se nessun bit di stato di errore è impostato
    return not lhs.eof() and not lhs.fail() and not lhs.bad() and lhs.good() and (istream::sentry(lhs)); //(istream::sentry(lhs)): Usa il costruttore sentry per gestire eventuali spazi bianchi iniziali nell'oggetto istream
}

string chiaveBug(istream& lhs){
    string str;
    bool escaped = false;
    bool esci = true;
    while (esci){
        char current;
        lhs.get(current);
        if (!lhs) {
            esci = false;
        }
        else{
            esci = true;
        }
        if(escaped){
            if (current == '"'){
                str.push_back(current);
            }
            else if (current == '\\'){
                str.push_back(current);
            }
            else{
                str.push_back(current);
            }
            escaped = false;
        }
        else{
            if(current == '"' and (str.empty() or str.back() != '\\')){ //qui eccezione non riconosciuta?
                esci = false;
            }
            else{
                if (current == '\\'){
                    escaped = true;
                }
                str.push_back(current);
            }
        }
    }
    return str;
}

void parse_dictionary(istream& lhs, json& rhs){
    rhs.set_dictionary();
    char current;
    skip(lhs, current);
    if(current == '}'){
        return void(); // do nothing for empty dictionary
    }
    else{
        assert(lhs.putback(current));
    }
    while(current != '}' and checkValidStream(lhs)){
        skip(lhs, current);
        if (current != '"'){
            throw json_exception{"Errore nel parsing, virgolette mancanti"};
        } 
        else{
            string key = chiaveBug(lhs);
            skip(lhs, current);
            if (current != ':') {
                throw json_exception{"Errore nel parsing, due punti mancanti"};
            } 
            else{
                json j;
                lhs >> j;
                rhs.insert(make_pair(key, j));
                if(!(lhs >> current) or ((current != ',') and (current != '}'))){
                    throw json_exception{"Errore nel parsing Dizio, carattere inaspettato"};
                }
            }
        }
    }
}

void parseString(istream& lhs, json& rhs){
    rhs.set_string(chiaveBug(lhs));
}

void parse_list(istream& lhs, json& rhs){
    rhs.set_list();
    char current;
    bool guardia;
    skip(lhs, current);
    if(current == ']'){
        guardia = false;
    }
    else{
        assert(lhs.putback(current));
        guardia = true;
    }
    while(checkValidStream(lhs) and guardia){
        json j;
        lhs >> j;
        rhs.push_back(j);
        if(!(lhs >> current)){
            throw json_exception{"Errore nel parsingList"};
        }
        else{
            if(current == ','){
                guardia = true;
            }
            else if(current == ']'){
                guardia = false;
            }
            else{
                throw json_exception{"Errore nel parsingList"};
            }
        }
    }
}

void parseTrue(istream& lhs, json& rhs){
    if(lhs.get() == 'r' && lhs.get() == 'u' && lhs.get() == 'e' && !isalnum(lhs.peek())) {
        rhs.set_bool(true);
    } 
    else{
        throw json_exception{"true not recognized"};
    }
}

void parseFalse(istream& lhs, json& rhs){
    if(lhs.get() == 'a' && lhs.get() == 'l' && lhs.get() == 's' && lhs.get() == 'e' && !isalnum(lhs.peek())) {
        rhs.set_bool(false);
    } 
    else{
        throw json_exception{"false not recognized"};
    }
}

void parseNumber(istream& lhs, json& rhs, char& current){
    double num;
    assert(lhs.putback(current));
    lhs >> num;
    rhs.set_number(num);
}

void parseNull(istream& lhs, json& rhs){
    if (lhs.get() == 'u' && lhs.get() == 'l' && lhs.get() == 'l' && !isalnum(lhs.peek())){
        rhs.set_null();
    } else {
        throw json_exception{"null not recognized"};
    }
}

istream& parse(istream& lhs, json& rhs){
    char current;
    skip(lhs, current);
    if(current == '"') {
        parseString(lhs, rhs);
    }
    else if(current == 'n'){
        parseNull(lhs, rhs);
    }
    else if(current == 't'){
        parseTrue(lhs, rhs);
    }
    else if(current == 'f'){
        parseFalse(lhs, rhs);
    }
    else if((current >= '0' and current <= '9') or current == '-' or current == '.' or current == '+') {
        parseNumber(lhs, rhs, current);
    }
    else if(current == '['){
        parse_list(lhs, rhs);
    }
    else if(current == '{'){
        parse_dictionary(lhs, rhs);
    }
    else{
        throw json_exception{"invalid input"};
    }
    return lhs;
}

istream& operator>>(istream& lhs, json& rhs){
    if(not checkValidStream(lhs)){
        throw json_exception{"stream sbagliato / file vuoto"};
    } 
    else {
        return parse(lhs, rhs);
    }
}

json& json::operator[](string const& str){
    if(!is_dictionary()){
        throw json_exception{"Non è dizionario"};
    }
    for(auto it = json::begin_dictionary(); it != json::end_dictionary(); ++it){
        if(str == it->first){
            return it->second;
        }
    }
    this->insert(make_pair(str, json()));
    return this->pimpl->Dizionario.tailJson->info.second;
}

json const& json::operator[](string const& str) const{
    if(!is_dictionary()){
        throw json_exception{"Non è dizionario operator[]"};
    }
    for(auto it = json::begin_dictionary(); it != json::end_dictionary(); ++it){
        if(str == it->first){
            return it->second;
        }
    }
    throw json_exception{"Chiave non presente"};
}
