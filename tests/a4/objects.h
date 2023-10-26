#pragma once

#include <string>

#include "gc.h"

class Object : public Collectable {
 public:
  Object() { allocated++; }

  virtual ~Object() { deallocated++; }

  static int64_t getAlive() { return (allocated - deallocated); }

  Object* lhs = nullptr;
  Object* rhs = nullptr;

 protected:
  virtual void follow(CollectedHeap& heap) override {
    if (lhs) heap.markSuccessors(lhs);
    if (rhs) heap.markSuccessors(rhs);
  };

  friend class CollectedHeap;

 private:
  static size_t allocated;
  static size_t deallocated;
};

class Integer final : public Object {
 public:
  Integer(int val) : val(val) {}

  size_t getSize() override { return sizeof(int); }
  std::string str() override { return std::to_string(val); }

  ~Integer() { val = -183534883; }

  int val;
};

class String final : public Object {
 public:
  String(std::string val) : val(val) {}

  size_t getSize() override { return sizeof(std::string) + val.capacity(); }
  std::string str() override { return val; }

  ~String() { val = "jijewriower89u33ijioejidjoder"; }

  std::string val;
};
