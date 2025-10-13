#pragma once
#include "Windows.h"
#include "iostream"

template <typename T>
struct unwrap_VMValueue {
    using type = T;
};

template<typename T>
class VMValue1 {
public:
    operator const T() const { return (T)((uintptr_t)storage - (uintptr_t)this); }
    void operator=(const T& value) { storage = (T)((uintptr_t)value + (uintptr_t)this); }
    const T operator->() const { return operator const T(); }
    T get() { return operator const T(); }
    void set(const T& value) { operator=(value); }

private:
    T storage;
};

template<typename T>
class VMValue2 {
public:
    operator const T() const { return (T)((uintptr_t)this - (uintptr_t)storage); }
    void operator=(const T& value) { storage = (T)((uintptr_t)this - (uintptr_t)value); }
    const T operator->() const { return operator const T(); }
    T get() { return operator const T(); }
    void set(const T& value) { operator=(value); }

private:
    T storage;
};

template<typename T>
class VMValue3 {
public:
    operator const T() const { return (T)((uintptr_t)this ^ (uintptr_t)storage); }
    void operator=(const T& value) { storage = (T)((uintptr_t)value ^ (uintptr_t)this); }
    const T operator->() const { return operator const T(); }
    T get() { return operator const T(); }
    void set(const T& value) { operator=(value); }

private:
    T storage;
};

template<typename T>
class VMValue4 {
public:
    operator const T() const { return (T)((uintptr_t)this + (uintptr_t)storage); }
    void operator=(const T& value) { storage = (T)((uintptr_t)value - (uintptr_t)this); }
    const T operator->() const { return operator const T(); }
    T get() { return operator const T(); }
    void set(const T& value) { operator=(value); }

private:
    T storage;
};