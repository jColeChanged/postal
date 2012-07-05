#include <iostream>
#include <list>
#include <string>
#include <map>
#include "helpers/strings.h"
#include "helpers/pattern.h"
#include "mail_messages/MailTable.h"

const int MAX_SENT = 20;
const int MAX_MSG_LENGTH = 500;

using namespace std;

typedef std::list<std::string> Responses;
typedef std::map<std::string, std::string> Bindings;

void postalInputHandler(Responses&, MailTable&, string, string);
void sendMessageHandler(Responses&, MailTable&, string, string);
void unsendMessageHandler(Responses&, MailTable&, string, string);
void readMessageHandler(Responses&, MailTable&, string);
void checkMessagesHandler(Responses&, MailTable&, string);
void sentMessagesHandler(Responses&, MailTable&, string);


void postalInputHandler(Responses& responses, 
			MailTable& table, 
			string from, 
			string message) {
  if (startsWith(message, "/postal help")) {
    std::cout << "Entered help.";
  }
  else if (startsWith(message, "/postal send")) {
    sendMessageHandler(responses, table, from, message);
  }
  else if (startsWith(message, "/postal check")) {
    checkMessagesHandler(responses, table, from);
  }
  else if (startsWith(message, "/postal sentlist")) {
    sentMessagesHandler(responses, table, from);
  }
  else if (startsWith(message, "/postal unsend")) {
    unsendMessageHandler(responses, table, from, message);
  }
  else if (startsWith(message, "/postal read")) {
    readMessageHandler(responses, table, from);   
  }
  else if (startsWith(message, "/postal ignore"))
    {
      std::cout << "Entered ignore.";
    }
  else if (startsWith(message, "/postal ignorelist"))
    {
      std::cout << "Entered ignorelist.";
    }
  else if (startsWith(message, "/postal unignore"))
    {
      std::cout << "Entered unignore.";
    }
  return;
}


  /*
  MAIL_ITEMS items;
  std::string header("POSTAL - IM the offline.\nSend me a message and I'll give it to someone else when they join the room. I try to keep things private so please interact with me via IMs.\n");
  std::string help("Commands:"                                         "\n"
  "/postal help"                                                       "\n" 
  "/postal send [user] [message]"                                      "\n"
       "\tYou can only have 30 sent messages at any one time."         "\n"
  "/postal sentlist"                                                   "\n"
       "\tReturns a list of the sent messages and their IDs."          "\n"
  "/postal unsend [message_id]"                                        "\n"
       "\tReturns a list of unread sent messages."                     "\n"
  "/postal read"                                                       "\n"
  "/postal ignore [user]"                                              "\n"
  "/postal ignorelist"                                                 "\n"
  "/postal unignore [user]"                                            "\n");
  std::cout << help << std::endl; */

void sendMessageHandler(Responses &responses, 
			MailTable &table, 
			string from, 
			string message) {
  if (MAX_SENT <= table.getMailFrom(from).size()) {
    responses.push_back("In order to prevent abuse, there is a limit on the "
      "number of messages which can be placed in escrow. You have reached "
      "this limit. Before you can send additional messages you must either "
      "(a) wait for your messages to be read or (b) use the '/postal unsend' "
      " command to remove a message from escrow.");
    return;
  }
  Bindings binds = matchPattern("/postal send ?user ?*body", message);
  if (isFailedMatch(binds)) {
    responses.push_back("Postal expects messages in the form '/postal send "
     "<user> <message ...>'.");
    return;
  }
  string to = binds.find("?user")->second;
  string body = binds.find("?*body")->second;
  if (MAX_MSG_LENGTH <= body.length()) {
    responses.push_back("In order to prevent abuse, there is a limit on the "
      "length of messages. You have passed this limit. This postal system is "
      "more like twitter than email.");
    return;
  }
  table.addMailItem(from, to, body);
  return;
}
void unsendMessageHandler(Responses &responses, 
			  MailTable &table, 
			  string from, 
			  string message) {
  Bindings binds = matchPattern("/postal unsend ?id", message);
  if (isFailedMatch(binds) || !isNumber(binds.find("?id")->second)) {
    responses.push_back("/postal unsend expects an id you own to be provided.");
    return;
  }
  int id = stringToInt(binds.find("?id")->second);
  MailItem item = table.getMailItemKey(id);
  if (item.from == from) {
    responses.push_back("Message removed.");
    table.popMailItemKey(id);
  }
  else {
    responses.push_back("/postal unsend expects an id you own to be provided.");
  }
  return;
}  
void checkMessagesHandler(Responses& responses, MailTable&table, string user) {
  int size = table.getMailTo(user).size();
  if (size) {
    responses.push_back(user + " you have " + intToString(size) + " unread "
      "message(s). To read a message you can use '/postal read'");
  }
  return;
}

void readMessageHandler(Responses &responses, MailTable &table, string user) {
  if (table.getMailTo(user).size()) {
    MailItem item = table.popMailItemTo(user);
    responses.push_back(item.from + " said: " + item.body);
  }
  else {
    responses.push_back("You don't have any unread messages.");
  }
  return;
}

void sentMessagesHandler(Responses &responses, MailTable &table, string from) {
  list<MailItem> sent = table.getMailFrom(from);
  string trunucated;
  string id;
  string to;
  for (list<MailItem>::const_iterator i = sent.begin(); i != sent.end(); i++) {
    trunucated = (*i).body.substr(0, 30);
    id = intToString((*i).primaryKey);
    to = (*i).to;
    responses.push_back("ID: " + id +  " To: " + to + " MSG: " + trunucated + 
      "...");
  }
  return;
}
    
    
      
#define CONSOLEMAIL_APP
#ifdef CONSOLEMAIL_APP
int main(void)
{
  MailTable table;
  Responses responses;
  
  cout << "To change your username use '/user username'. Doing so will cause "
       << "the user to 'enter the room' as the user you specify.\n";
  
  string username = "user1";
  string in;
  string out;
  while (true) {
    cout << "POSTAL> ";
    getline(cin, in);
    Bindings binds = matchPattern("/user ?user", in);
    if (!isFailedMatch(binds)) {
      username = binds.find("?user")->second;
      responses.push_back(username + " has joined the room.");
      checkMessagesHandler(responses, table, username);
    }
    postalInputHandler(responses, table, username, in);
    while (!responses.empty()) {
      out = responses.front();
      responses.pop_front();
      cout << out << endl;
    }
  }
  return 0;
}
#endif
