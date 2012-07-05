#include "MailItem.h"

MailItem::MailItem(std::string from, std::string to, std::string body, unsigned long primaryKey) {
  this->from = from;
  this->to = to;
  this->body = body;
  this->primaryKey = primaryKey;
}

bool operator<(const MailItem &lhs, const MailItem &rhs) {
  return lhs.primaryKey < rhs.primaryKey;
}
bool operator>(const MailItem &lhs, const MailItem &rhs) {
  return rhs < lhs;
}
bool operator>=(const MailItem &lhs, const MailItem &rhs) {
  return !(lhs < rhs);
}
bool operator<=(const MailItem &lhs, const MailItem &rhs) {
  return !(lhs > rhs);
}
bool operator==(const MailItem &lhs, const MailItem &rhs) {
  return (rhs <= lhs) && (rhs >= lhs);
}
bool operator!=(const MailItem &lhs, const MailItem &rhs) {
  return !(lhs == rhs);
}
