all:
	g++ src/postal/postal.cpp src/postal/helpers/strings.cpp src/postal/helpers/lists.cpp src/postal/helpers/pattern.cpp src/postal/mail_messages/MailTable.cpp src/postal/mail_messages/MailItem.cpp -o postal