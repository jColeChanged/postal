#ifndef MAILITEM_H
#define MAILITEM_H
#include <string>

class MailItem
{
public:
  MailItem();
  MailItem(std::string from, std::string to, std::string body, int primaryKey);
  ~MailItem(){};
  std::string from;
  std::string to;
  std::string body;
  int primaryKey;
};

bool operator<(const MailItem &lhs, const MailItem &rhs);
bool operator>(const MailItem &lhs, const MailItem &rhs);
bool operator<=(const MailItem &lhs, const MailItem &rhs);
bool operator>=(const MailItem &lhs, const MailItem &rhs);
bool operator==(const MailItem &lhs, const MailItem &rhs);
bool operator!=(const MailItem &lhs, const MailItem &rhs);

#endif
