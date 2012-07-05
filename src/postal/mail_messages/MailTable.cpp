#include "MailTable.h"
#include <algorithm>

using namespace std;

MailTable::MailTable() {
  list<MailItem> mail;
  this->mail = mail;
}

MailTable::MailTable(list<MailItem> mail) {
  this->mail = mail;
}

unsigned long MailTable::getUnusedKey() {
  unsigned long unusedKey = 0;
  for (list<MailItem>::iterator i = (*this).mail.begin(); i != (*this).mail.end(); i++) {
    if (i->primaryKey != unusedKey) {
      return unusedKey;
    }
    else {
      unusedKey++;
    }
  }
  return unusedKey;
}

void MailTable::addMailItem(string from, string to, string body) {
  unsigned long key = this->getUnusedKey();
  mail.push_back(MailItem(from, to, body, key));
  mail.sort();
}

list<MailItem> MailTable::getMailFrom(const string &from)
{
  list<MailItem> items_to_return;
  for (list<MailItem>::const_iterator iter = (*this).mail.begin(); 
       iter != (*this).mail.end(); 
       iter++) {
    if (iter->from == from) {
      items_to_return.push_back(*iter);
    }
  }
  return items_to_return;
}

list<MailItem> MailTable::getMailTo(const string &to)
{
  list<MailItem> items_to_return;
  for (list<MailItem>::const_iterator iter = (*this).mail.begin(); 
       iter != (*this).mail.end(); 
       iter++) {
    if (iter->to == to) {
      items_to_return.push_back(*iter);
    }
  }
  return items_to_return;
}

MailItem MailTable::popMailItemTo(const string &to) {
  MailItem item = this->getMailTo(to).back();
  this->mail.remove(item);
  return item;
}
