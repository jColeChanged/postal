#include <iostream>
#include <list>
#include <string>
#include <algorithm>
#include <map>
#include "helpers/strings.h"
#include "helpers/pattern.h"
#include "mail_messages/MailTable.h"
#include "mail_messages/MailItem.h"
#include "tinyxml/tinyxml.h"

const int MAX_SENT = 20;
const int MAX_MSG_LENGTH = 500;
const int MAX_IGNORES = 20;
const char * IGNORES_FILE = "postalignores.xml";
const char * MAILS_FILE = "postalmail.xml";

using namespace std;

typedef std::list<std::string> Responses;
typedef std::map<std::string, std::string> Bindings;
typedef std::map<std::string, std::list<std::string> > IgnoreMap;

void persistIgnoreSave(const char * pFileName, IgnoreMap &ignores);
void persistIgnoreLoad(const char * pFileName, IgnoreMap &ignores);

void persistMailSave(const char * pFileName, MailTable &table);
void persistMailLoad(const char * pFileName, MailTable &table);

void postalInputHandler(Responses&, MailTable&, IgnoreMap&, string, string);
void sendMessageHandler(Responses&, MailTable&, IgnoreMap&, string, string);
void unsendMessageHandler(Responses&, MailTable&, string, string);
void readMessageHandler(Responses&, MailTable&, string);
void checkMessagesHandler(Responses&, MailTable&, string);
void sentMessagesHandler(Responses&, MailTable&, string);
void aboutMessageHandler(Responses&);
void helpMessageHandler(Responses&);
void ignoreMessageHandler(Responses&, IgnoreMap&, string, string);
void unignoreMessageHandler(Responses&, IgnoreMap&, string, string);
void ignoreListHandler(Responses&, IgnoreMap&, string);


void postalInputHandler(Responses& responses, 
			MailTable& table,
			IgnoreMap& ignores,
			string from, 
			string message) {
  if (startsWith(message, "/postal help")) {
    helpMessageHandler(responses);
  }
  else if (startsWith(message, "/postal about")) {
    aboutMessageHandler(responses);
  }
  else if (startsWith(message, "/postal send")) {
    sendMessageHandler(responses, table, ignores, from, message);
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
  else if (startsWith(message, "/postal ignore")) {
    ignoreMessageHandler(responses, ignores, from, message);
  }
  else if (startsWith(message, "/postal ignoring")) {
    ignoreListHandler(responses, ignores, from);
  }
  else if (startsWith(message, "/postal unignore")) {
    unignoreMessageHandler(responses, ignores, from, message);
  }
  return;
}

void sendMessageHandler(Responses &responses, 
			MailTable &table, 
			IgnoreMap &ignores,
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
  IgnoreMap::iterator iter = ignores.find(to);
  if (iter != ignores.end()) {
    list<string>::iterator iterTwo = find(iter->second.begin(),
					  iter->second.end(),
					  from);
    if (iterTwo != iter->second.end()) {
	responses.push_back("Sorry, that person has you on ignore.");
	return;
    }
  }
  if (MAX_MSG_LENGTH <= body.length()) {
    responses.push_back("In order to prevent abuse, there is a limit on the "
      "length of messages. You have passed this limit. This postal system is "
      "more like twitter than email.");
    return;
  }
  table.addMailItem(from, to, body);
  persistMailSave(MAILS_FILE, table);
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
    persistMailSave(MAILS_FILE, table);
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
    persistMailSave(MAILS_FILE, table);
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

void aboutMessageHandler(Responses &responses) {
  std::string header("POSTAL - IM the offline.\nSend me a message and I'll give it to someone else when they join the room. I try to keep things private so please interact with me via IMs.\n");
  std::string info("I was made by Joshua Cole. He sometimes makes and sells Camfrog bot plugins. You can learn more about him and the plugins he has made at 'http://jcolessite.appspot.com/projects/camfrog/'.");
  responses.push_back(header);
  responses.push_back(info);
}

void helpMessageHandler(Responses &responses) {
  string help_message = "Commands:\n"
    "/postal help\n/postal send [user] [message]\n"
    "\tYou can only have 30 sent messages at any one time.\n/postal sentlist"
    "\n\tReturns a list of the sent messages and their IDs.";
  string help_message_cont = "/postal unsend [message_id]\n\tReturns a list"
    " of unread sent messages.\n/postal read\n/postal ignore [user]\n/postal "
    "ignoring\n/postal unignore [user]\n/postal about";
  responses.push_back(help_message);
  responses.push_back(help_message_cont);
}
      
void ignoreMessageHandler(Responses &responses,
			  IgnoreMap &ignores, 
			  string from,
			  string message) {
  Bindings binds = matchPattern("/postal ignore ?user", message);
  if (isFailedMatch(binds)) {
    responses.push_back("/postal ignore expects input in the form '/postal "
      "ignore <username>'");
    return;
  }
  string toIgnore = binds.find("?user")->second;
  list<string> ignoreList;
  if (ignores.find(from) == ignores.end()) {
    ignores[from] = ignoreList;
  }
  if (MAX_IGNORES <= ignores[from].size()) {
    responses.push_back("To prevent abuse there is a limit to the number of "
     " people you can ignore. To free up more ignores you can remove people "
     " from your ignore list with '/postal uningore <username>'");
    return;
  }
  ignores[from].push_back(toIgnore);
  responses.push_back(toIgnore + " can no longer send you messages.");
  persistIgnoreSave(IGNORES_FILE, ignores);
}

void unignoreMessageHandler(Responses &responses, 
			   IgnoreMap &ignores, 
			   string from, 
			   string message) {
  IgnoreMap::iterator iter = ignores.find(from);
  // assuming that it is possible for find to pass even w/ empty list as val
  if (iter == ignores.end()) {
    responses.push_back("You don't have anyone on ignore.");
    return;
  }
  Bindings binds = matchPattern("/postal unignore ?user", message);
  if (isFailedMatch(binds)) {
    responses.push_back("/postal unignore expects input in the form '/postal "
      "unignore <user>'");
    return;
  }
  ignores[from].remove(binds.find("?user")->second);
  responses.push_back("No longer ignoring " + binds.find("?user")->second);
  persistIgnoreSave(IGNORES_FILE, ignores);
}
  
void ignoreListHandler(Responses &responses, IgnoreMap &ignores, string from) {
  IgnoreMap::iterator iter = ignores.find(from);
  if (iter == ignores.end()) {
    responses.push_back("You don't have anyone on ignore.");
    return; 
  }
  list<string> ignoreList = iter->second;
  for (list<string>::iterator i = ignoreList.begin();
       i != ignoreList.end();
       i++) {
    responses.push_back("Ignoring " + *i);
  }
}

#define CONSOLEMAIL_APP
#ifdef CONSOLEMAIL_APP
int main(void)
{
  MailTable table;
  Responses responses;
  IgnoreMap ignores;
  
  cout << "To change your username use '/user username'. Doing so will cause "
       << "the user to 'enter the room' as the user you specify.\n";
  persistIgnoreLoad(IGNORES_FILE, ignores);
  persistMailLoad(MAILS_FILE, table);
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
    postalInputHandler(responses, table, ignores, username, in);
    while (!responses.empty()) {
      out = responses.front();
      responses.pop_front();
      cout << out << endl;
    }
  }
  return 0;
}
#endif

void persistIgnoreSave(const char * pFileName, IgnoreMap &ignores) {
  TiXmlDocument doc;
  
  TiXmlDeclaration * declaration = new TiXmlDeclaration("1.0", "", "");
  doc.LinkEndChild(declaration);
  
  TiXmlElement * root = new TiXmlElement("IgnoreList");
  doc.LinkEndChild(root);
  
  IgnoreMap::iterator iter;
  for (iter=ignores.begin(); iter != ignores.end(); iter++) {
    const string user = iter->first;
    TiXmlElement * userElement = new TiXmlElement("User");
    root->LinkEndChild(userElement);
    userElement->SetAttribute("name", user.c_str());
    list<string>::iterator iter2;
    for (iter2=iter->second.begin(); iter2 != iter->second.end(); iter2++) {
      TiXmlElement * ignoredUserElement = new TiXmlElement("User");
      userElement->LinkEndChild(ignoredUserElement);
      ignoredUserElement->SetAttribute("name", (*iter2).c_str());
    }
  }
  doc.SaveFile(pFileName);
}

void persistIgnoreLoad(const char * pFileName, IgnoreMap &ignores) {
  TiXmlDocument doc(pFileName);
  if (!doc.LoadFile()) {
    return;
  }
  TiXmlHandle hDoc(&doc);
  TiXmlElement * pElem;
  TiXmlHandle hRoot(0);
  
  // make sure the file has a valid root
  pElem = hDoc.FirstChildElement().Element();
  if (!pElem) {
    return;
  }
  hRoot = TiXmlHandle(pElem);
  
  TiXmlHandle elementHandler(0);
  TiXmlElement * ignoreNode;
  TiXmlElement * userNode;
  userNode = hRoot.FirstChildElement().Element();
  for (userNode; userNode; userNode=userNode->NextSiblingElement()) {
    elementHandler = TiXmlHandle(userNode);
    // make sure the userNode has children
    ignoreNode = elementHandler.FirstChildElement().Element();
    if (ignoreNode) {
      const char * ignorer = userNode->Attribute("name");
      list<string> ignoreList;
      for (ignoreNode; 
	   ignoreNode; 
	   ignoreNode=ignoreNode->NextSiblingElement()) {
	const char * ignoree = ignoreNode->Attribute("name");
	ignoreList.push_back(ignoree);
      }
      ignores[ignorer] = ignoreList;
    }
  }
}     

void persistMailSave(const char * pFileName, MailTable &table) {
  TiXmlDocument doc;
  
  TiXmlDeclaration * declaration = new TiXmlDeclaration("1.0", "", "");
  doc.LinkEndChild(declaration);
  
  TiXmlElement * root = new TiXmlElement("MailTable");
  doc.LinkEndChild(root);
  
  list<MailItem> mail = table.getAllMail();
  list<MailItem>::iterator iter;
  for (iter=mail.begin(); iter != mail.end(); iter++) {
    TiXmlElement * mailItem = new TiXmlElement("MailItem");
    root->LinkEndChild(mailItem);
    mailItem->SetAttribute("from", (*iter).from.c_str());
    mailItem->SetAttribute("to", (*iter).to.c_str());
    mailItem->SetAttribute("body", (*iter).body.c_str());
    mailItem->SetAttribute("id", intToString((*iter).primaryKey).c_str());
  }
  doc.SaveFile(pFileName);
}

void persistMailLoad(const char * pFileName, MailTable &table)
{
  TiXmlDocument doc(pFileName);
  if (!doc.LoadFile()) {
    return;
  }
  TiXmlHandle hDoc(&doc);
  TiXmlElement * pElem;
  TiXmlHandle hRoot(0);
  
  // make sure the file has a valid root
  pElem = hDoc.FirstChildElement().Element();
  if (!pElem) {
    return;
  }
  hRoot = TiXmlHandle(pElem);
  
  list<MailItem> mail;
  TiXmlElement * mailNode;
  mailNode = hRoot.FirstChildElement().Element();
  for (mailNode; mailNode; mailNode=mailNode->NextSiblingElement()) {
    const char * from = mailNode->Attribute("from");
    const char * to = mailNode->Attribute("to");
    const char * body = mailNode->Attribute("body");
    const char * id = mailNode->Attribute("id");
    mail.push_back(MailItem(from, to, body, stringToInt(id)));
  }
  mail.sort();
  table = MailTable(mail);
}
