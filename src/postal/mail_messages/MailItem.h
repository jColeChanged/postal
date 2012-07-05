#ifndef MAILITEM_H
#define MAILITEM_H
#include <string>

class MailItem
{
public:
  MailItem(std::string from, std::string to, std::string body, 
	   unsigned long primaryKey);
  ~MailItem(){};
  std::string from;
  std::string to;
  std::string body;
  unsigned long primaryKey;
};

bool operator<(const MailItem &lhs, const MailItem &rhs);
bool operator>(const MailItem &lhs, const MailItem &rhs);
bool operator<=(const MailItem &lhs, const MailItem &rhs);
bool operator>=(const MailItem &lhs, const MailItem &rhs);
bool operator==(const MailItem &lhs, const MailItem &rhs);
bool operator!=(const MailItem &lhs, const MailItem &rhs);

#endif
