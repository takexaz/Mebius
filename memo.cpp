class Singleton {
private:
    static Singleton* _instance;
    Singleton(bool is_cout, bool is_cerr) {
        return;
    }
    ~Singleton() {
        return;
    }
public:
    static Singleton* get_instance() {
        if (!_instance) _instance = new Singleton();
        return _instance;
    }
};