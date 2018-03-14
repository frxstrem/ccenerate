//
// Created by fredrik on 13.03.18.
//

#ifndef CCENERATE_GENERATORS_H
#define CCENERATE_GENERATORS_H

#include <iostream>
#include <memory>
#include <vector>
#include <utility>
#include <string>
#include <sstream>

#include <cppast/cpp_class.hpp>
#include <cppast/cpp_member_variable.hpp>
#include <cppast/cpp_member_function.hpp>

class generator {
protected:
  cppast::cpp_entity_index &index;

public:
  generator(cppast::cpp_entity_index &index) : index(index) {}

  virtual ~generator() = default;

  generator(const generator &) = delete;

  generator &operator=(const generator &) = delete;

  virtual void write(std::ostream &) const {}

  virtual void add_member_variable(const cppast::cpp_member_variable &) {}

  virtual void add_member_function(const cppast::cpp_member_function &) {}
};

class generator_list {
private:
  cppast::cpp_entity_index &index;
  std::vector<std::unique_ptr<generator>> generators;

public:
  generator_list(cppast::cpp_entity_index &index) : index(index) {};

  ~generator_list() = default;

  generator_list(const generator_list &) = delete;

  generator_list(generator_list &&list) : index(list.index), generators(std::move(list.generators)) {}

  generator_list &operator=(const generator_list &) = delete;

  template<typename T, typename... Args>
  void emplace(Args &&...args) {
    generators.push_back(std::make_unique<T>(std::ref(index), std::forward<Args>(args)...));
  }

  size_t size() const { generators.size(); }

#define wrap_method(name) template<typename... Args> void name(Args &&...args) { \
  for(const std::unique_ptr<generator> &g: generators) g->name(std::forward<Args>(args)...); }
#define wrap_method_const(name) template<typename... Args> void name(Args &&...args) const { \
  for(const std::unique_ptr<generator> &g: generators) g->name(std::forward<Args>(args)...); }

  wrap_method_const(write)

  wrap_method(add_member_variable)

  wrap_method(add_member_function)

#undef wrap_method
#undef wrap_method_const

  void write(std::ostream &out) const {
    for (const std::unique_ptr<generator> &g : generators) {
      g->write(out);
    }
  }

  virtual void add_member_variable(const cppast::cpp_member_variable &) const {
    for (const std::unique_ptr<generator> &g : generators) {

    }
  }

  virtual void add_member_function(const cppast::cpp_member_function &) const {}
};

class json_generator : public generator {
private:
  const cppast::cpp_class &cls;
  std::ostringstream to_json_body, from_json_body;

public:
  json_generator(cppast::cpp_entity_index &index, const cppast::cpp_class &cls) : generator(index), cls(cls) {}

  void write(std::ostream &out) const override;

  void add_member_variable(const cppast::cpp_member_variable &) override;
};

#endif // CCENERATE_GENERATORS_H
