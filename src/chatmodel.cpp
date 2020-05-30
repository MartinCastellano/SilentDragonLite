// Copyright 2019-2020 The Hush developers
// GPLv3
#include "chatmodel.h"
#include "settings.h"
#include "ui_confirm.h"
#include "controller.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_requestContactDialog.h"
#include "addressbook.h"
#include "ui_contactrequest.h"
#include <QtWidgets>
#include <QUuid>
#include "DataStore/DataStore.h"

using namespace std; 

ContactRequest contactRequest = ContactRequest();

ChatModel::ChatModel(std::map<QString, ChatItem> chatItems)
{
    this->chatItems = chatItems;
}

ChatModel::ChatModel(std::vector<ChatItem> chatItems)
{
   this->setItems(chatItems);
}

std::map<QString, ChatItem> ChatModel::getItems()
{
    return this->chatItems;
}

void ChatModel::setItems(std::map<QString, ChatItem> items)
{
    this->chatItems = chatItems;
}

void ChatModel::setItems(std::vector<ChatItem> items)
{
    for(ChatItem c : items)
    {
        this->chatItems[ChatIDGenerator::getInstance()->generateID(c)] = c; //this->generateChatItemID(c)] = c;
    }
}

void ChatModel::clear()
{
    this->chatItems.clear();
}

void ChatModel::addMessage(ChatItem item)
{
    QString key = ChatIDGenerator::getInstance()->generateID(item); //this->generateChatItemID(item);
    this->chatItems[key] = item;
}

void ChatModel::addMessage(QString timestamp, ChatItem item)
{
    QString key = ChatIDGenerator::getInstance()->generateID(item);//this->generateChatItemID(item);
    timestamp = "0";
    this->chatItems[key] = item;
}

void ChatModel::showMessages()
{
    for(auto &c : this->chatItems)
    {
      //  qDebug() << c.second.toChatLine();
    }
          
}

void ChatModel::addAddressbylabel(QString address, QString label)
{
    this->AddressbyLabelMap[address] = label;
}

QString ChatModel::Addressbylabel(QString address)
{
    for(auto& pair : this->AddressbyLabelMap)
    {

    }

    if(this->AddressbyLabelMap.count(address) > 0)
    {
        return this->AddressbyLabelMap[address];
    }

    return QString("0xdeadbeef");
}
     
void MainWindow::renderContactRequest(){

        Ui_requestDialog requestContact;
        QDialog dialog(this);
        requestContact.setupUi(&dialog);
        Settings::saveRestore(&dialog);

        QString icon;
        auto theme = Settings::getInstance()->get_theme_name();
        if (theme == "dark" || theme == "midnight") {
           icon = ":/icons/res/unknownWhite.png";
        }else{
            icon = ":/icons/res/unknownBlack.png";
        }

         QPixmap unknownWhite(icon);
         QIcon addnewAddrIcon(unknownWhite);
       
        
       

       QStandardItemModel* contactRequest = new QStandardItemModel();

            for (auto &c : DataStore::getChatDataStore()->getAllNewContactRequests())



            {

                QStandardItem* Items = new QStandardItem(QString("Unknown Sender"));
                contactRequest->appendRow(Items);
                requestContact.requestContact->setModel(contactRequest);
                
                Items->setData(QIcon(addnewAddrIcon),Qt::DecorationRole);
                requestContact.requestContact->setIconSize(QSize(40,50));
                requestContact.requestContact->setUniformItemSizes(true);
                requestContact.requestContact->show();
                requestContact.zaddrnew->setVisible(false);
                requestContact.zaddrnew->setText(c.second.getAddress());

            }

            QStandardItemModel* contactRequestOld = new QStandardItemModel();

             for (auto &p : AddressBook::getInstance()->getAllAddressLabels())
              for (auto &c : DataStore::getChatDataStore()->getAllOldContactRequests())
            {
               if (p.getPartnerAddress() == c.second.getRequestZaddr())
               {
                QStandardItem* Items = new QStandardItem(p.getName());
                contactRequestOld->appendRow(Items);
                requestContact.requestContactOld->setModel(contactRequestOld);
                requestContact.zaddrold->setVisible(false);
                requestContact.zaddrold->setText(c.second.getAddress());
                requestContact.requestContactOld->show();
               }else{}
            }
       

        QObject::connect(requestContact.requestContact, &QTableView::clicked, [&] () {

    for (auto &c : DataStore::getChatDataStore()->getAllRawChatItems()){
        QModelIndex index = requestContact.requestContact->currentIndex();
        QString label_contact = index.data(Qt::DisplayRole).toString();
        QStandardItemModel* contactMemo = new QStandardItemModel();
           
        if  ((c.second.isOutgoing() == false) && (requestContact.zaddrnew->text() == c.second.getAddress()) && (c.second.getType() != "Cont"))
        
        {

          QStandardItem* Items = new QStandardItem(c.second.getMemo());
             contactMemo->appendRow(Items);
            requestContact.requestMemo->setModel(contactMemo);   
            requestContact.requestMemo->show();
           
            requestContact.requestCID->setText(c.second.getCid());
            requestContact.requestCID->setVisible(false);
            requestContact.requestZaddr->setText(c.second.getRequestZaddr());
            requestContact.requestMyAddr->setText(c.second.getAddress());
            }else{}
        }
    
    
            
   });

   QObject::connect(requestContact.requestContactOld, &QTableView::clicked, [&] () {

    for (auto &c : DataStore::getChatDataStore()->getAllRawChatItems()){
       /* QModelIndex index = requestContact.requestContactOld->currentIndex();
        QString label_contactold = index.data(Qt::DisplayRole).toString();*/
        QStandardItemModel* contactMemo = new QStandardItemModel();
           
        if  ((c.second.isOutgoing() == false) && (requestContact.zaddrold->text() == c.second.getAddress()) && (c.second.getType() != "Cont"))
        
        {

          QStandardItem* Items = new QStandardItem(c.second.getMemo());
             contactMemo->appendRow(Items);
            requestContact.requestMemo->setModel(contactMemo);   
            requestContact.requestMemo->show();
           
            requestContact.requestCID->setText(c.second.getCid());
            requestContact.requestCID->setVisible(false);
            requestContact.requestZaddr->setText(c.second.getRequestZaddr());
            requestContact.requestMyAddr->setText(c.second.getAddress());
            }else{}
        }
    
    
            
   });
   
  QObject::connect(requestContact.pushButton, &QPushButton::clicked, [&] () {

            QString cid = requestContact.requestCID->text();
            auto addr = requestContact.requestZaddr->text().trimmed();
            QString newLabel = requestContact.requestLabel->text().trimmed();
            auto myAddr = requestContact.requestMyAddr->text().trimmed();

            QString avatar = QString(":/icons/res/") + requestContact.comboBoxAvatar->currentText() + QString(".png");

             if (addr.isEmpty() || newLabel.isEmpty()) 
        {
             QMessageBox::critical(
                this, 
                QObject::tr("Address or Label Error"), 
                QObject::tr("Address or Label cannot be empty"), 
                QMessageBox::Ok
                );
            return;
        }

        // Test if address is valid.
        if (!Settings::isValidAddress(addr)) 
        {
          QMessageBox::critical(
                this, 
                QObject::tr("Address Format Error"), 
                QObject::tr("%1 doesn't seem to be a valid hush address.").arg(addr), 
                QMessageBox::Ok
            );
            return;
        } 

                qDebug()<<"Beginn kopiert" <<cid << addr << newLabel << myAddr;
                AddressBook::getInstance()->addAddressLabel(newLabel, addr, myAddr, cid, avatar);
                  rpc->refreshContacts(
                  ui->listContactWidget);

                  QMessageBox::information(this, "Added Contact","successfully added your new contact. You can now Chat with this contact");  
            
    });

 dialog.exec();

}

void ChatModel::addCid(QString tx, QString cid)
{
    this->cidMap[tx] = cid;
}

void ChatModel::addrequestZaddr(QString tx, QString requestZaddr)
{
    this->requestZaddrMap[tx] = requestZaddr;
}

void ChatModel::addconfirmations(QString tx, int confirmation)
{
    this->confirmationsMap[tx] = confirmation;
}

QString ChatModel::getCidByTx(QString tx)
{
    for(auto& pair : this->cidMap)
    {

    }

    if(this->cidMap.count(tx) > 0)
    {
        return this->cidMap[tx];
    }

    return QString("0xdeadbeef");
}

QString ChatModel::getConfirmationByTx(QString tx)
{
    for(auto& pair : this->confirmationsMap)
    {

    }

    if(this->confirmationsMap.count(tx) > 0)
    {
        return this->confirmationsMap[tx];
    }

    return QString("0xdeadbeef");
}

QString ChatModel::getrequestZaddrByTx(QString tx)
{
    for(auto& pair : this->requestZaddrMap)
    {

    }

    if(this->requestZaddrMap.count(tx) > 0)
    {
        return this->requestZaddrMap[tx];
    }

    return QString("0xdeadbeef");
}

void ChatModel::killCidCache()
{
    this->cidMap.clear();
}

void ChatModel::killrequestZaddrCache()
{
    this->requestZaddrMap.clear();
}

void ChatModel::killConfirmationCache()
{
    this->confirmationsMap.clear();
}

QString MainWindow::createHeaderMemo(QString type, QString cid, QString zaddr,  int version=0, int headerNumber=1)
{
    
    QString header="";
    QJsonDocument j;
    QJsonObject h;
    // We use short keynames to use less space for metadata and so allow
    // the user to send more actual data in memos
    h["h"]   = headerNumber;    // header number
    h["v"]   = version;         // HushChat version
    h["z"]   = zaddr;           // zaddr to respond to
    h["cid"] = cid;             // conversation id
    h["t"] = type;       // Memo or incoming contact request

    j.setObject(h);
    header = j.toJson();
    qDebug() << "made header=" << header;
    return header;
   
}

// Create a Tx from the current state of the Chat page. 
Tx MainWindow::createTxFromChatPage() {
   Tx tx;
    CAmount totalAmt;
    // For each addr/amt in the Chat tab
  {
       
        QString amtStr = "0";
        CAmount amt;  

         
            amt = CAmount::fromDecimalString("0");
            totalAmt = totalAmt + amt;

  
    for(auto &c : AddressBook::getInstance()->getAllAddressLabels())

     if (ui->contactNameMemo->text().trimmed() == c.getName()) {
     
            QString cid = c.getCid();
            QString myAddr = c.getMyAddress();
            QString type = "Memo";
            QString addr = c.getPartnerAddress();
           

        QString hmemo= createHeaderMemo(type,cid,myAddr);

             /////////User input for chatmemos
        QString memounencrypt = ui->memoTxtChat->toPlainText().trimmed();

  /////////We convert the user input from QString to unsigned char*, so we can encrypt it later
        int length = memounencrypt.length();

          char *sequence = NULL;
         sequence = new char[length+1];
         strncpy(sequence, memounencrypt.toLocal8Bit(), length +1);

//////////////////////////////////////////////////Lets create Alice keys for the conversation///////////////////////////////////

                /////////////////Alice Pubkey 
    #define MESSAGEAP ((const unsigned char *) "Ioesd")///////////static atm, in future we will use the CID here
    #define MESSAGEAP_LEN 5

    unsigned char alice_publickey[crypto_secretstream_xchacha20poly1305_KEYBYTES];

       crypto_generichash(alice_publickey, sizeof alice_publickey,
                   MESSAGEAP, MESSAGEAP_LEN,
                   NULL, 0);
                    QString alice = QString::fromLocal8Bit((char*)alice_publickey);
                   qDebug()<<"Alice Pubkey : "<<alice;


    /////////////////Alice Secretkey 

       #define MESSAGEAS ((const unsigned char *) "Hallo")///////////static atm, in future we will use the Passphrase here
        #define MESSAGEAS_LEN 5

    unsigned char alice_secretkey[crypto_secretstream_xchacha20poly1305_KEYBYTES];

      crypto_generichash(alice_secretkey, sizeof alice_secretkey,
                   MESSAGEAS, MESSAGEAS_LEN,
                   NULL, 0);

 /////////////////Bob Pubkey that Alice creates 
     #define MESSAGEBAP ((const unsigned char *) "Hal11")///////////static atm, in future we will use the CID here
    #define MESSAGEBAP_LEN 5

    unsigned char bob_publickey[crypto_secretstream_xchacha20poly1305_KEYBYTES];

    crypto_generichash(bob_publickey, sizeof bob_publickey,
                   MESSAGEBAP, MESSAGEBAP_LEN,
                   NULL, 0);

    qDebug()<<"Alice version of Bobs Pubkey created";


         ////////////Now we create shared keys for the conversation//////////////////////////////

    unsigned char server_rx[crypto_kx_SESSIONKEYBYTES], server_tx[crypto_kx_SESSIONKEYBYTES];
    /* Generate the server's key pair */
    crypto_kx_keypair(alice_publickey, alice_secretkey);

    /* Prerequisite after this point: the client's public key must be known by the server */

    /* Compute two shared keys using the client's public key and the server's secret key.
     server_rx will be used by the server to receive data from the client,
     server_tx will by used by the server to send data to the client. */
    if (crypto_kx_server_session_keys(server_rx, server_tx,
                                      alice_publickey, alice_secretkey, bob_publickey) != 0) {
    /* Suspicious client public key, bail out */

}

  ////////////Now lets encrypt the message Alice send to Bob//////////////////////////////
    #define MESSAGE (const unsigned char *) sequence
    #define MESSAGE_LEN length
    #define CIPHERTEXT_LEN (MESSAGE_LEN + crypto_secretstream_xchacha20poly1305_ABYTES)

    
        crypto_secretstream_xchacha20poly1305_state state;
        /////The Header must be known by both, so we can use alice or bobs pubkey here

    //unsigned char header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];

    unsigned char c1[CIPHERTEXT_LEN];

    /* Shared secret key required to encrypt/decrypt the stream */
    crypto_secretstream_xchacha20poly1305_keygen(alice_publickey);

    /* Set up a new stream: initialize the state and create the header */
    ////////////Bobs pubkey will be use as header here, and alice_publickey as Key. Just for testing, we will switch to server_tx here //////////////////////////////
    crypto_secretstream_xchacha20poly1305_init_push(&state, bob_publickey, alice_publickey);

   ////Now encrypt the Message///////////////////////////////////
    crypto_secretstream_xchacha20poly1305_push
    (&state, c1, NULL, MESSAGE, MESSAGE_LEN, NULL, 0, 0);

    ///////////Ciphertext is now in c1 as unsigned char*, we need it as QString, to send it as memo to Bob//////////////////////

            /////This is not working. It seems the QString has not exact the ciphertext in it. It will not decrypt with it////////////////
                QString memo( reinterpret_cast< char* >( c1 ) );

    qDebug()<<"Size QString with encrypted data :"<< memo.length(); ////We check the length here, to compare it with the length our QString 

                ///////Just for testing we convert the unsigned char* to std::string, to see if we can decrypt,and that works.////////////

                std::string encryptedMemo(reinterpret_cast<char*>(c1), CIPHERTEXT_LEN);


    qDebug()<<"Size std::string with encrypted data :"<< encryptedMemo.length(); ////We check the length here, to compare it with the length our QString

        

 /////////////////////////////////Now we create Bobs keys, just for testing at this place. If the encryption/decryption works we put it in Controller.cpp (RefreshTransactions)
    
     /////////////////Bob Pubkey/////////////////////////////////
    #define MESSAGEBAP1 ((const unsigned char *) "Hal12")///////////static atm, in future we will use the CID here
    #define MESSAGEBAP1_LEN 5

    unsigned char bob1_publickey[crypto_secretstream_xchacha20poly1305_KEYBYTES];

    crypto_generichash(bob1_publickey, sizeof bob1_publickey,
                   MESSAGEBAP1, MESSAGEBAP1_LEN,
                   NULL, 0);

    qDebug()<<"Bobs Pubkey created";

       /////////////////Bob Secretkey 
    #define MESSAGEBS ((const unsigned char *) "Hal11")///////////static atm, in future we will use the Passphrase here
    #define MESSAGEBS_LEN 5

    unsigned char bob_secretkey[crypto_secretstream_xchacha20poly1305_HEADERBYTES];

    crypto_generichash(bob_secretkey, sizeof bob_secretkey,
                   MESSAGEBS, MESSAGEBS_LEN,
                   NULL, 0);

    qDebug()<<"Bobs Pubkey created";

    /////////////////Alice Pubkey bob creates
   #define MESSAGEA121 ((const unsigned char *) "Ioesd")///////////static atm, in future we will use the CID here
    #define MESSAGEAP121_LEN 5

    unsigned char alice1_publickey[crypto_secretstream_xchacha20poly1305_KEYBYTES];

       crypto_generichash(alice1_publickey, sizeof alice1_publickey,
                   MESSAGEA121, MESSAGEAP121_LEN,
                   NULL, 0);

                   QString alice1 = QString::fromLocal8Bit((char*)alice1_publickey);
                   qDebug()<<"Alice Pubkey Bob create: "<<alice1;


    

////////////Now we create the shared keys for Bob///////////////////////////////////////////////

        unsigned char client_rx[crypto_kx_SESSIONKEYBYTES], client_tx[crypto_kx_SESSIONKEYBYTES];

        /* Generate the client's key pair */
        crypto_kx_keypair(bob1_publickey, bob_secretkey);

        /* Prerequisite after this point: the server's public key must be known by the client */

        /* Compute two shared keys using the server's public key and the client's secret key.
         client_rx will be used by the client to receive data from the server,
         client_tx will by used by the client to send data to the server. */
        if (crypto_kx_client_session_keys(client_rx, client_tx,
                                          bob1_publickey, bob_secretkey, alice1_publickey) != 0) {
         /* Suspicious server public key, bail out */
        } 



///////////////Now we simuldate the decryption. Our memo comes as QString, so we have to convert it again to unsigned char*////////////////////

        ////QString to char+ (not working, it will not decrypt)

          /*char *memoIncoming = NULL;
         memoIncoming = new char[lenght1+1];
         strncpy(memoIncoming, memo.toLocal8Bit(), lenght1+1);*/

           
///////////////////////////if we take the std::string (of the encryption output) instead of QString memo, it will decrypt////////

/////////////Convert std::string to unsigned char*/////////////////////
        unsigned char *pwa=new unsigned char[encryptedMemo.length()];
        strncpy((char *)pwa,encryptedMemo.c_str(), encryptedMemo.length());

//////We set the values to decrypt the message///////////////////////////////


        //////unsigned char* as message from std::string
         #define MESSAGE2 (const unsigned char *) pwa 
         ///////// length of the encrypted message
         #define CIPHERTEXT1_LEN  encryptedMemo.length()  
         ///////Message length is smaller then the encrypted message
         #define MESSAGE1_LEN encryptedMemo.length()  - crypto_secretstream_xchacha20poly1305_ABYTES 
         ///////we could use tags here 
         unsigned char tag;

            //////Set the length of the decrypted message
        unsigned char m[MESSAGE1_LEN];

                ///////In future we use the shared keys here//////
        /* Shared secret key required to encrypt/decrypt the stream */
        //crypto_secretstream_xchacha20poly1305_keygen(alice1_publickey);

        /* Set up a new stream: initialize the state and create the header */
        //crypto_secretstream_xchacha20poly1305_init_push(&state, alice_publickey, client_rx);

        ////////Again, we use Bobs pubkey as header and alice pubkey as key
        /* Decrypt the stream: initializes the state, using the key and a header */
        if (crypto_secretstream_xchacha20poly1305_init_pull(&state, bob_publickey, alice_publickey) != 0) {
        /* Invalid header, no need to go any further */
        }
          //////And decrypt it
        if (crypto_secretstream_xchacha20poly1305_pull
        (&state, m, NULL, &tag, MESSAGE2, encryptedMemo.length(), NULL, 0) != 0) {
         /* Invalid/incomplete/corrupted ciphertext - abort */
        }

            /////Our decrypted message is now in m. We need it as QString to render it
                /////Only the QString gives weird data, so convert first to std::string
            std::string decryptedMemo(reinterpret_cast<char*>(m),MESSAGE1_LEN);
              /////Now we can convert it to QString
            QString memodecrypt = QString::fromUtf8( decryptedMemo.data(), decryptedMemo.size());

         //////////////Give us the output of the decrypted message as debug to see if it was successfully
            qDebug()<<"OUT  decrypt:" << memodecrypt;      
     
     tx.toAddrs.push_back(ToFields{addr, amt, hmemo});
     tx.toAddrs.push_back(ToFields{addr, amt, memo});

     

         qDebug() << "pushback chattx";
   } 
   }

    tx.fee = Settings::getMinerFee();

     return tx;

     qDebug() << "ChatTx created";
}

void MainWindow::sendChat() {

////////////////////////////Todo: Check if a Contact is selected//////////

    // Create a Tx from the values on the send tab. Note that this Tx object
    // might not be valid yet.
 
   QString Name = ui->contactNameMemo->text();
   int sizename = Name.size();
        qDebug()<< sizename;
      if (ui->contactNameMemo->text().trimmed().isEmpty() || ui->memoTxtChat->toPlainText().trimmed().isEmpty()) {
     
        QMessageBox msg(QMessageBox::Critical, tr("You have to select a contact and insert a Memo"),
        tr("You have selected no Contact from Contactlist,\n")  + tr("\nor your Memo is empty"),
        QMessageBox::Ok, this);

        msg.exec();
        return;
    }

    int max = 512;
    QString chattext = ui->memoTxtChat->toPlainText();
    int size = chattext.size();

    if (size > max){
     
        QMessageBox msg(QMessageBox::Critical, tr("Your Message is too long"),
        tr("You can only write messages with 512 character maximum \n")  + tr("\n Please reduce your message to 512 character."),
        QMessageBox::Ok, this);

        msg.exec();
        return;
    }

    Tx tx = createTxFromChatPage();

    QString error = doSendChatTxValidations(tx);

    if (!error.isEmpty()) {
        // Something went wrong, so show an error and exit
        QMessageBox msg(QMessageBox::Critical, tr("Message Error"), error,
                        QMessageBox::Ok, this);

        msg.exec();

        // abort the Tx
        return;
        qDebug() << "Tx aborted";
    }

        // Create a new Dialog to show that we are computing/sending the Tx
        auto d = new QDialog(this);
        auto connD = new Ui_ConnectionDialog();
        connD->setupUi(d);
        QMovie *movie1 = new QMovie(":/img/res/silentdragonlite-animated.gif");;
        QMovie *movie2 = new QMovie(":/img/res/silentdragonlite-animated-dark.gif");;
        auto theme = Settings::getInstance()->get_theme_name();
        if (theme == "dark" || theme == "midnight") {
            movie2->setScaledSize(QSize(512,512));
            connD->topIcon->setMovie(movie2);
            movie2->start();
        } else {
            movie1->setScaledSize(QSize(512,512));
            connD->topIcon->setMovie(movie1);
            movie1->start();
        }

        connD->status->setText(tr("Please wait..."));
        connD->statusDetail->setText(tr("Your Message will be sent"));

        d->show();
        ui->memoTxtChat->clear();
        

        // And send the Tx
        rpc->executeTransaction(tx, 
            [=] (QString txid) { 
                ui->statusBar->showMessage(Settings::txidStatusMessage + " " + txid);

                connD->status->setText(tr("Done!"));
                connD->statusDetail->setText(txid);
                

                QTimer::singleShot(1000, [=]() {
                    d->accept();
                    d->close();
                    delete connD;
                    delete d;
                    
                  });
                
                // Force a UI update so we get the unconfirmed Tx
                rpc->refresh(true);
                ui->memoTxtChat->clear();

            },
            // Errored out
            [=] (QString opid, QString errStr) {
                ui->statusBar->showMessage(QObject::tr(" Tx ") % opid % QObject::tr(" failed"), 15 * 1000);
                
                d->accept();
                d->close();
                delete connD;
                delete d;

                if (!opid.isEmpty())
                    errStr = QObject::tr("The transaction with id ") % opid % QObject::tr(" failed. The error was") + ":\n\n" + errStr;            

                QMessageBox::critical(this, QObject::tr("Transaction Error"), errStr, QMessageBox::Ok);            
            }
        );

      //  rpc->refresh(true);
    }        

QString MainWindow::doSendChatTxValidations(Tx tx) {
    // Check to see if we have enough verified funds to send the Tx.

    CAmount total;
    for (auto toAddr : tx.toAddrs) {
        if (!Settings::isValidAddress(toAddr.addr)) {
            QString addr = (toAddr.addr.length() > 100 ? toAddr.addr.left(100) + "..." : toAddr.addr);
            return QString(tr("Recipient Address ")) % addr % tr(" is Invalid");
        }

        // This technically shouldn't be possible, but issue #62 seems to have discovered a bug
        // somewhere, so just add a check to make sure. 
        if (toAddr.amount.toqint64() < 0) {
            return QString(tr("Amount for address '%1' is invalid!").arg(toAddr.addr));
        }

        total = total + toAddr.amount;
    }
    total = total + tx.fee;

    auto available = rpc->getModel()->getAvailableBalance();

    if (available < total) {
        return tr("Not enough available funds to send this transaction\n\nHave: %1\nNeed: %2\n\nNote: Funds need 1 confirmations before they can be spent")
            .arg(available.toDecimalhushString(), total.toDecimalhushString());
    }

    return "";
}

void::MainWindow::addContact() 
{
    Ui_Dialog request;
    QDialog dialog(this);
    request.setupUi(&dialog);
    Settings::saveRestore(&dialog);

 try 
    {   
    bool sapling = true;
    rpc->createNewZaddr(sapling, [=] (json reply) {
        QString myAddr = QString::fromStdString(reply.get<json::array_t>()[0]);
        rpc->refreshAddresses();
        request.myzaddr->setText(myAddr);
        ui->listReceiveAddresses->insertItem(0, myAddr); 
        ui->listReceiveAddresses->setCurrentIndex(0);
        qDebug() << "new generated myAddr add Contact" << myAddr;
    });

    }catch(...)
       {

            
            qDebug() << QString("Caught something nasty with myZaddr Contact");
       }

        QString cid = QUuid::createUuid().toString(QUuid::WithoutBraces);

    QObject::connect(request.sendRequestButton, &QPushButton::clicked, [&] () {
        
        QString addr = request.zaddr->text();
        QString myAddr = request.myzaddr->text().trimmed();
        QString memo = request.memorequest->text().trimmed();
        QString avatar = QString(":/icons/res/") + request.comboBoxAvatar->currentText() + QString(".png");
        QString label = request.labelRequest->text().trimmed();


        contactRequest.setSenderAddress(myAddr);
        contactRequest.setReceiverAddress(addr);
        contactRequest.setMemo(memo);
        contactRequest.setCid(cid);
        contactRequest.setAvatar(avatar);
        contactRequest.setLabel(label);

    });
        
   QObject::connect(request.sendRequestButton, &QPushButton::clicked, this, &MainWindow::saveandsendContact);
  // QObject::connect(request.onlyAdd, &QPushButton::clicked, this, &MainWindow::saveContact);
        
    dialog.exec();
       
    rpc->refreshContacts(ui->listContactWidget);

}

void MainWindow::saveandsendContact()
{
        this->ContactRequest();
        
}

// Create a Tx for a contact Request 
Tx MainWindow::createTxForSafeContactRequest() 
{
    Tx tx; 
{
    CAmount totalAmt;
    QString amtStr = "0";
    CAmount amt;  
    amt = CAmount::fromDecimalString("0");
    totalAmt = totalAmt + amt;
   
            QString cid = contactRequest.getCid();
            QString myAddr = contactRequest.getSenderAddress();
            QString type = "Cont";
            QString addr = contactRequest.getReceiverAddress();

            QString hmemo= createHeaderMemo(type,cid,myAddr);
            QString memo = contactRequest.getMemo();

     
            tx.toAddrs.push_back(ToFields{addr, amt, hmemo});
            tx.toAddrs.push_back(ToFields{addr, amt, memo});
            qDebug() << "pushback chattx";
            tx.fee = Settings::getMinerFee();
        
}
        
    return tx;
    qDebug() << "RequestTx created";
}

void MainWindow::ContactRequest() {

    if (contactRequest.getReceiverAddress().isEmpty() || contactRequest.getMemo().isEmpty()) {
     
  // auto addr = "";
  //  if (! Settings::isZAddress(AddressBook::addressFromAddressLabel(addr->text()))) {
        QMessageBox msg(QMessageBox::Critical, tr("You have to select a contact and insert a Memo"),
        tr("You have selected no Contact from Contactlist,\n")  + tr("\nor your Memo is empty"),
        QMessageBox::Ok, this);

        msg.exec();
        return;
    }

    int max = 512;
    QString chattext = contactRequest.getMemo();;
    int size = chattext.size();

    if (size > max){
     
  // auto addr = "";
  //  if (! Settings::isZAddress(AddressBook::addressFromAddressLabel(addr->text()))) {
        QMessageBox msg(QMessageBox::Critical, tr("Your Message is too long"),
        tr("You can only write messages with 512 character maximum \n")  + tr("\n Please reduce your message to 512 character."),
        QMessageBox::Ok, this);

        msg.exec();
        return;
    }

  Tx tx = createTxForSafeContactRequest();

    QString error = doSendRequestTxValidations(tx);

    if (!error.isEmpty()) {
        // Something went wrong, so show an error and exit
        QMessageBox msg(QMessageBox::Critical, tr("Message Error"), error,
                        QMessageBox::Ok, this);

        msg.exec();

        // abort the Tx
        return;
        qDebug() << "Tx aborted";
    }

        // Create a new Dialog to show that we are computing/sending the Tx
        auto d = new QDialog(this);
        auto connD = new Ui_ConnectionDialog();
        connD->setupUi(d);
        QMovie *movie1 = new QMovie(":/img/res/silentdragonlite-animated.gif");;
        QMovie *movie2 = new QMovie(":/img/res/silentdragonlite-animated-dark.gif");;
        auto theme = Settings::getInstance()->get_theme_name();
        if (theme == "dark" || theme == "midnight") {
            movie2->setScaledSize(QSize(512,512));
            connD->topIcon->setMovie(movie2);
            movie2->start();
        } else {
            movie1->setScaledSize(QSize(512,512));
            connD->topIcon->setMovie(movie1);
            movie1->start();
        }

        connD->status->setText(tr("Please wait..."));
        connD->statusDetail->setText(tr("Your contact request will be sent"));

        d->show();
        ui->memoTxtChat->clear();

        // And send the Tx
        rpc->executeTransaction(tx, 
            [=] (QString txid) { 
                ui->statusBar->showMessage(Settings::txidStatusMessage + " " + txid);

                connD->status->setText(tr("Done!"));
                connD->statusDetail->setText(txid);

                QTimer::singleShot(1000, [=]() {
                    d->accept();
                    d->close();
                    delete connD;
                    delete d;
                    
                  });

                  /////Add this contact after we sent the request

        QString addr = contactRequest.getReceiverAddress();
        QString newLabel = contactRequest.getLabel();
        QString myAddr = contactRequest.getSenderAddress();
        QString cid = contactRequest.getCid();
        QString avatar = contactRequest.getAvatar();
        
        if (addr.isEmpty() || newLabel.isEmpty()) 
        {
            QMessageBox::critical(
                this, 
                QObject::tr("Address or Label Error"), 
                QObject::tr("Address or Label cannot be empty"), 
                QMessageBox::Ok
                );
            return;
        }

        // Test if address is valid.
        if (!Settings::isValidAddress(addr)) 
        {
            QMessageBox::critical(
                this, 
                QObject::tr("Address Format Error"), 
                QObject::tr("%1 doesn't seem to be a valid hush address.").arg(addr), 
                QMessageBox::Ok
            );
            return;
        } 

        ///////Todo: Test if label allready exist!

        ////// Success, so show it
        AddressBook::getInstance()->addAddressLabel(newLabel, addr, myAddr, cid, avatar);
        rpc->refreshContacts(
        ui->listContactWidget);
        QMessageBox::information(
            this, 
            QObject::tr("Added Contact"), 
            QObject::tr("successfully added your new contact").arg(newLabel), 
            QMessageBox::Ok
          
        );
        return;
                // Force a UI update so we get the unconfirmed Tx
              //  rpc->refresh(true);
                ui->memoTxtChat->clear();
                rpc->refresh(true);
                rpc->refreshContacts(
                ui->listContactWidget);

            },
            // Errored out
            [=] (QString opid, QString errStr) {
                ui->statusBar->showMessage(QObject::tr(" Tx ") % opid % QObject::tr(" failed"), 15 * 1000);
                
                d->accept();
                d->close();
                delete connD;
                delete d;

                if (!opid.isEmpty())
                    errStr = QObject::tr("The transaction with id ") % opid % QObject::tr(" failed. The error was") + ":\n\n" + errStr;            

                QMessageBox::critical(this, QObject::tr("Transaction Error"), errStr, QMessageBox::Ok);            
            }
        );
      
    }        


QString MainWindow::doSendRequestTxValidations(Tx tx) {
    // Check to see if we have enough verified funds to send the Tx.

    CAmount total;
    for (auto toAddr : tx.toAddrs) {
        if (!Settings::isValidAddress(toAddr.addr)) {
            QString addr = (toAddr.addr.length() > 100 ? toAddr.addr.left(100) + "..." : toAddr.addr);
            return QString(tr("Recipient Address ")) % addr % tr(" is Invalid");
        }

        // This technically shouldn't be possible, but issue #62 seems to have discovered a bug
        // somewhere, so just add a check to make sure. 
        if (toAddr.amount.toqint64() < 0) {
            return QString(tr("Amount for address '%1' is invalid!").arg(toAddr.addr));
        }

        total = total + toAddr.amount;
    }
    total = total + tx.fee;

    auto available = rpc->getModel()->getAvailableBalance();

    if (available < total) {
        return tr("Not enough available funds to send this transaction\n\nHave: %1\nNeed: %2\n\nNote: Funds need 1 confirmations before they can be spent")
            .arg(available.toDecimalhushString(), total.toDecimalhushString());
    }

    return "";
}


