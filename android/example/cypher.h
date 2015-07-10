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

            //char buffer[32];
            QDataStream in(&file);
            in >> data;

        }
        return data;
    }

    void write_data(const QMap<QString,QString> data) const {
        QFile file(file_name);
        file.open(QIODevice::WriteOnly);
        QDataStream out(&file);

        out << data;
    }

    QString get_file_name() const {
        return file_name;
    }

private:
    cryptstate crypt_state;
    QString file_name;
};

#endif // CYPHER_H

