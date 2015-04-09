#ifndef __CORE_SINGLETON_HPP__
#define __CORE_SINGLETON_HPP__


namespace sg
{

    template<typename T>
    class Singleton
    {
    public:
        static T&   Instance()      { static T _instance; return _instance; }
        static T*   Instance_ptr()  { return &Instance(); }

    public:
        Singleton() {}
        virtual ~Singleton() {}

    private:
        Singleton(const Singleton&);
        Singleton& operator=(const Singleton&);
    };


}






#endif
