# Chat BOT [https://zatturight.com/fastbot/products/detail/85]
CharBOTは、BOT内蔵のブラウザ版ChatGPTに対してWebSocketから命令を行うことで、ChatGPT APIを利用しなくてもそれらしい動きをしてくれるツールです。<br>
ChatBOTはWebSocketServerとして動作します。Client側は「ws://xxx.xxx.xxx.xxx:55554」で接続します。<br>
セキュアな通信も可能です。chatbot.exeと同じ階層にサーバ証明書（server.key, server.crt）を配置後にchatbot.exeを起動します。Client側は「wss://xxx.xxx.xxx.xxx:55554」で接続します。<br>
<br>
ChatBOTは、以下の機能があります。

### １．ログイン機能
#### 1.1 概要
email / passwordによるログインを行います。<br>
画像認証が出た場合のみ、ChatBOTの内蔵ブラウザから手動で突破する必要があります。<br>
※Googleアカウント、Microsoftアカウント、Appleアカウントによるログインは未対応

##### 注意）ログイン・ログアウトを頻繁に行うと、BOTと判断されてしまい丸１日ログインできなくなります。Cookieを保持しているため、ChatBOTを再起動してもログイン状態は継続されるので、頻繁にログイン・ログアウトを行う必要はありません。

#### 1.2 通信データ仕様
以下のJson構文をChatBOTへ送ります。

```
{
    "id":1
    "email":"xxxxxxxx"
    "password":"xxxxxxxx"
}
```
ChatBOTから以下の応答があります。
```
・ログイン画面へ遷移中
{
    "id":1
    "result":"Wait"
    "reason":"ログイン施行中..."
}
・EMail文字列を入力したとき
{
    "id":1
    "result":"Wait"
    "reason":Email入力中..."
}
・パスワード文字列を入力したとき
{
    "id":1
    "result":"Wait"
    "reason":パスワード入力中..."
}
・パスワード文字列を入力したとき
{
    "id":1
    "result":"Wait"
    "reason":パスワード入力中..."
}
・ログイン完了時
{
    "id":1
    "result":"OK"
    "reason":ログイン成功"
}
・既にログイン済みの場合
{
    "id":1
    "result":"OK"
    "reason":既にログイン済みです"
}
・要求のアカウントとは別のアカウントでログイン中の場合
{
    "id":1
    "result":"Wait"
    "reason":別のアカウントにログイン中のため、ログアウト中..."
}
・画像認証が出たとき
{
    "id":1
    "result":"Wait"
    "reason":画像認証が出たので、手動で突破してください。"
}
```

### ２．チャット履歴取得機能
#### 2.1 概要
ChatBOTがログイン状態であればチャット履歴を取得できます。ログイン状態では無いときは、取得できません。

#### 2.2 通信データ仕様
以下のJson構文をChatBOTへ送ります。
```
{
    "id":2
}
```
ChatBOTから以下の応答があります。
```
・チャット履歴取得中...
{
    "id":2
    "result":"Wait"
    "history": [
        {
            "url":"xxxxxxxx"
            "subject":"xxxxxxx"
        },
        {
            "url":"xxxxxxxx"
            "subject":"xxxxxxx"
        }
    ]
}
・チャット履歴取得完了
{
    "id":2
    "result":"OK"
    "history": [
        {
            "url":"xxxxxxxx"
            "subject":"xxxxxxx"
        },
        {
            "url":"xxxxxxxx"
            "subject":"xxxxxxx"
        }
    ]
}
```

### ３．メッセージ送信
#### 3.1 概要
ChatBOTに対してメッセージを送信します。ChatGPTが応答している最中は、定期的にメッセージを返答します。

#### 3.2 通信データ仕様
以下のJson構文をChatBOTへ送ります。
```
{
    "id":3
    "message":"xxxxxxxxxxx\nxxxxxxxxxx"
}
※改行コード(\n)使用可能
```
ChatBOTから以下の応答があります。
```
・ChatGPTがメッセージ応答を返しているとき。
{
    "id":3
    "result":"Wait"
    "reply":[
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
    ]
}
※reply配列の先頭が「あなた」のメッセージで、それ以降はChatGPTのメッセージです
※メッセージは、HTMLタグを含んでいます

・ChatGPTがメッセージ応答し終わったとき。
{
    "id":3
    "result":"OK"
    "reply":[
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
    ]
}
※reply配列の先頭が「あなた」のメッセージで、それ以降はChatGPTのメッセージです
※メッセージは、HTMLタグを含んでいます

```

### ４．ログアウト機能
#### 4.1 概要
ChatGPTのアカウントをログアウトします。

#### 4.2 通信データ仕様
以下のJson構文をChatBOTへ送ります。
```
{
    "id":4
}
```
ChatBOTから以下の応答があります。
```
{
    "id":4
    "result":"OK"
    "reason":"ログアウト完了"
}
```

### ５．チャット履歴表示機能
#### 5.1 概要
チャット履歴を開き、すべてのメッセージを取得します。

#### 5.2 通信データ仕様
以下のJson構文をChatBOTへ送ります。
```
{
    "id":5
    "url":"xxxxxxxxxxx"
}
※チャット履歴より取得したURLを"url"に設定します
```
ChatBOTから以下の応答があります。
```
{
    "id":5
    "reply":[
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
    ]
}
※先頭から順に「あなた」、「ChatGPT」・・・の順になります
```
