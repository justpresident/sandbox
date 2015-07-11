#ifndef CYPHER_H
#define CYPHER_H

#include <rijndael.h>
#include <string.h>

#include <QMap>
#include <QFileInfo>
#include <QFile>
#include <QDataStream>
#include <QByteArray>

typedef struct cryptstate {
  RIJNDAEL_context ctx;
  UINT8 iv[RIJNDAEL_BLOCKSIZE];
  int mode;
} Crypt__Rijndael;

class Cypher {
public:
    Cypher(const QString file_name, QString key)
        :file_name(file_name)
    {
        while(key.length() < 32) {
            key.append("~");
        }

        UINT8 key_buffer[128];
        size_t key_len = 0;
        memset(key_buffer, 0, 128);
        key = key.left(128);
        for (int i = 0; i < key.length(); ++i) {
            key_buffer[i] = key.at(i).toLatin1();
            key_len++;
        }

        crypt_setup(key_buffer, key_len);
    }
    ~Cypher() {};

    void crypt_setup(const UINT8 *key, size_t key_len) {
        struct cryptstate crypt_state;
        crypt_state.mode = MODE_CBC;

        memset(crypt_state.iv, 0, RIJNDAEL_BLOCKSIZE);
        rijndael_setup(&crypt_state.ctx, key_len, key);
    }

    QMap<QString,QString> read_data() {
        QFileInfo fi(file_name);
        QMap<QString,QString> data;
        if (fi.exists() && fi.size() > 0) {
            QFile file(file_name);
            file.open(QIODevice::ReadOnly);
            // read data from file to byteArray
            QByteArray byteArray = file.readAll();
            file.close();

            //remove padding length
            char pad_length = byteArray.at(0);
            byteArray.remove(0, 1);

            //decrypt data

            //remove padding
            byteArray.remove(byteArray.length()-pad_length, pad_length);

            //deserialize data from byteArray
            QDataStream in(byteArray);
            in.setVersion(QDataStream::Qt_5_4);
            in >> data;

        }
        return data;
    }

    void write_data(const QMap<QString,QString> data) const {
        QByteArray byteArray;
        QDataStream out(&byteArray, QIODevice::ReadWrite);
        out.setVersion(QDataStream::Qt_5_4);

        // serialize data to byteArray
        out << data;
        out.setDevice(0);

        // write padding
        char pad_length = 16 - (byteArray.length() % 16);
        for (qint8 i = 0; i < pad_length; ++i)
            byteArray.append('~');

        // encrypt byteArray


        QFile file(file_name);
        file.open(QIODevice::WriteOnly);
        // write padding length to file
        file.write(&pad_length, 1);

        // write data from byteArray to file
        file.write(byteArray);
        file.close();
    }

    QString get_file_name() const {
        return file_name;
    }

private:
    cryptstate crypt_state;
    QString file_name;
};

#endif // CYPHER_H

