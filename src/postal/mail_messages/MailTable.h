#ifndef MAILTABLE_H
#define MAILTABLE_H

#include <string>
#include <list>
#include "MailItem.h"

class MailTable {
public:
  MailTable();
  MailTable(std::list<MailItem> mail);
  void addMailItem(std::string from, std::string to, std::string body);
  std::list<MailItem> getMailFrom(const std::string &from);
  std::list<MailItem> getMailTo(const std::string &to);
  MailItem popMailItemTo(const std::string &to);
private:
  unsigned long getUnusedKey();
  std::list<MailItem> mail;
};
#endif
