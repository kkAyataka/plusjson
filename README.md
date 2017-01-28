plusjson
========

C++ JSON parser and builder.

## Implicit cast method

    // Set value to JSON object.
    plusjson::Value v = plusjson::Object();
    v["null"]    = plusjson::Null();
    v["boolean"] = true;
    v["string"]  = "string";
    v["number"]  = 1.1;
    v["array"]   = plusjson::Array();
    v["object"]  = plusjson::Object();

    // Get value from JSON object.
    plsujson::Null    n   = v["null"];
    plusjson::Boolean b   = v["boolean"];
    plusjson::Number  num = v["number"];
    plusjson::String  str = v["string"];
    plusjson::Array   arr = v["array"];
    plusjson::Object  obj = v["object"];

    // If a type is mismatch, throw exception
    plusjson::number v = v["string"]; // exception!

## Convenient operator

## ToDo
* Enumのリネーム
  * kTypeNullなどにする
  * isの実装をマクロ化する
  * inlineの指定
  * path指定によるファイル読み込み/書き出し
  * デフォルトget関数
* テスト
  * [x] 引数ありコンストラクタ
    * [ ] typeがあってる
    * [ ] 値が格納されている
    * [ ] Object、Array、Number、String、Boolean、Nullで正しいこと
  * [ ] 引数なしコンストラクラ
    * [ ] Nullであること
  * [x] コピーコンストラクタ
    * [ ] typeが正しいこと
    * [ ] 値が正しいこと
    * [ ] Object、Array、Number、String、Boolean、Nullで正しいこと
  * [x] =演算子
    * [ ] typeが正しいこと
    * [ ] 値が正しいこと
    * [ ] Object、Array、Number、String、Boolean、Nullで正しいこと
  * [x] ==演算子
    * [ ] 比較できること
    * [ ] Object、Array、Number、String、Boolean、Nullで正しいこと
    * [ ] Object、Array、Stringは値で比較していること
  * [x] !=演算子
    * [ ] 比較できること
  * [x] get_typeメソッド
    * [ ] typeが取得できること
  * [x] isメソッド
    * [ ] typeの確認ができること
  * [x] getメソッド
    * [ ] 値が取得できること
    * [ ] typeが違う場合例外が発生すること
  * [ ] []演算子
    * [ ] Objectに対しchar \*指定で値が取れること
    * [ ] Objectに対しString指定で値が取れること
    * [ ] Arrayにstd::size_t指定で値が取れること
    * [ ] typeがミスマッチしている場合、例外が発生すること
  * [ ] 型キャスト
    * [ ] Object、Array、Number、String、Boolean、Nullにキャスト出来ること
  * [ ] to_json_string
  * Null
    * [ ] to_null
    * [ ] to_json_string
  * Noolean
    * [ ] to_boolean
      * [ ] true/false
    * [ ] to_json_string
      * [ ] true/false
  * Number
    * [ ] to_number
      * [ ] Array/Objectの途中
      * [ ] Arrayの最後
      * [ ] Objectの最後
    * [ ] to_json_string
      * [ ] 有効桁が16桁
      * [ ] マイナス/プラス(符号なし)
      * [ ] 0-9で始まる数字
  * String
    * [ ] to_string
      * [ ] エスケープ
    * [ ] to_json_string
      * [ ] エスケープ
  * Array
    * [ ] to_array
      * [ ] パース失敗で例外が飛ぶ
    * [ ] to_json_string
      * [ ] readableフラグ
  * Object
    * [ ] to_array
      * [ ] パース失敗で例外が飛ぶ
    * [ ] to_json_string
      * [ ] readableフラグ
  * from_json_string
    * Object、Array、Number、String、Boolean、Nullの評価ができるように
  * to_json_string
    * Object、Array、Number、String、Boolean、Nullの評価ができるように
