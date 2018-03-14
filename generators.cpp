#include "./generators.h"

#include <iostream>
#include <stack>
#include <cppast/cpp_entity_kind.hpp>

static std::string string_literal(const std::string &raw);

static std::string get_full_name(const cppast::cpp_entity &e);

void json_generator::write(std::ostream &out) const {
  std::string class_name = get_full_name(cls);

  out << "void to_json(::nlohmann::json &j, const " << class_name << " &v) {\n"
      << "  j = json::object();\n"
      << to_json_body.str()
      << "}\n"
      << "\n"
      << "void from_json(const ::nlohman::json &j, " << class_name << " &v) {\n"
      << from_json_body.str()
      << "}\n";
}

void json_generator::add_member_variable(const cppast::cpp_member_variable &var) {
  std::string json_name = string_literal(var.name());

  if (auto name_attr_opt = cppast::has_attribute(var, "json::name")) {
    const cppast::cpp_attribute &name_attr = name_attr_opt.value();

    if (name_attr.arguments().has_value()) {
      const cppast::cpp_token_string &args = name_attr.arguments().value();
      std::vector<cppast::cpp_token> tokens{args.begin(), args.end()};

      if (tokens.size() > 0 && tokens[0].kind == cppast::cpp_token_kind::string_literal) {
        json_name = tokens[0].spelling;
      }
    }
  }

  to_json_body << "  j["
               << json_name
               << "] = v."
               << var.name()
               << ";\n";

  from_json_body << "  v."
                 << var.name()
                 << " = j.at("
                 << json_name
                 << ").get<decltype(v."
                 << var.name()
                 << ")>();\n";
}

std::string string_literal(const std::string &raw) {
  return "\"" + raw + "\"";
}

std::string get_full_name(const cppast::cpp_entity &e) {
  std::ostringstream ss;
  std::stack<std::string> parts;

  parts.push(e.name());
  auto p = e.parent();
  while (p) {
    const cppast::cpp_entity &pe = p.value();
    switch (pe.kind()) {
      case cppast::cpp_entity_kind::class_t:
      case cppast::cpp_entity_kind::namespace_t:
        if (!pe.name().empty())
          parts.push(pe.name());
        break;
    }
    p = p.value().parent();
  }

  while (!parts.empty()) {
    ss << "::" << parts.top();
    parts.pop();
  }

  return ss.str();
}
