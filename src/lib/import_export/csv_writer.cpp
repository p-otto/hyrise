#include "csv_writer.hpp"

#include <fstream>
#include <string>
#include <vector>

#include "types.hpp"

namespace opossum {

CsvWriter::CsvWriter(const std::string& file, const ParseConfig& config) : _config(config) {
  _stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  _stream.open(file);
}

/*
 * Escapes each quote character with an escape symbol.
 */
std::string CsvWriter::escape(const std::string& string) {
  std::string result(string);
  size_t next_pos = 0;
  while (std::string::npos != (next_pos = result.find(_config.quote, next_pos))) {
    result.insert(next_pos, 1, _config.escape);
    // Has to jump 2 positions ahead because a new character had been inserted.
    next_pos += 2;
  }
  return result;
}

void CsvWriter::write(const AllTypeVariant& value) {
  if (_current_col_count > 0) {
    _stream << _config.separator;
  }

  _write_value(value);
  ++_current_col_count;
}

void CsvWriter::end_line() {
  _stream << _config.delimiter;
  _current_col_count = 0;
}

void CsvWriter::_write_value(const AllTypeVariant& value) {
  if (variant_is_null(value)) return;

  if (value.type() == typeid(std::string)) {
    _write_string_value(type_cast<std::string>(value));
    return;
  }

  _stream << value;
}

void CsvWriter::_write_string_value(const std::string& value) {
  /**
   * We put an the quotechars around any string value by default
   * as this is the only time when a comma (,) might be inside a value.
   * This might consume more space, however it speeds the program as it
   * does not require additional checks.
   * If we start allowing more characters as delimiter, we should change
   * this behaviour to either general quoting or checking for "illegal"
   * characters.
   */
  _stream << _config.quote;
  _stream << escape(value);
  _stream << _config.quote;
}

}  // namespace opossum
