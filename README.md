wav_lua
=======

### 概要
wavファイルに記録された波形をLuaのスクリプトに渡して加工させ、
その結果をwavファイルに書き出すプログラムです。
Lua5.3.1で動作確認しています。

入出力のwavファイルは省略できるので、正弦波などの音声を直接生成することも可能です。

### コマンドライン

```
$ wav_lua 入力wavファイル 出力wavファイル Luaスクリプトファイル [Luaに渡すパラメータ...]
```

入力/出力wavファイルは"-"を指定することで、ファイルを参照しなくなります。

### Luaスクリプトの書き方

3引数のprocess関数をグローバルで定義してください。
第一引数が波形データ、第二引数がサンプルレート[Hz]、第三引数がコマンドラインで指定されたパラメータです。

#### 波形データ
各チャンネルごとの波形が順に格納されたテーブルです。
波形は、-1～1の実数のテーブルで表されています。

#### サンプルレート
wavファイルから読み取ったサンプルレートです。

#### コマンドラインで指定されたパラメータ
コマンドラインで指定されたパラメータがテーブルで渡されます。
コマンド(wav_luaなど)、入出力wavファイル名、Luaスクリプトファイル名は含まれません。

#### 戻り値
0個～3個の値を返せます。
最初の値は入力と同じ形式の波形データのテーブルです。
各チャンネルのテーブルの「長さ」が同じでないと弾かれます。
値を指定しない場合、ファイルは出力されません。

2番目の値はサンプルレートです。省略すると入力と同じになります。

3番目の値は1サンプルのビット数です。8と16に対応しています。省略すると入力と同じになります。

### スクリプト
* am.lua
  AM変調を行います。
* dsb.lua
  DSB変調を行います。
* fm.lua
  FM変調を行います。
* lpf.lua
  抵抗両終端型ローパスフィルタ回路のシミュレーションを行います。
* pam.lua
  PAM変調を行います。
* pwm.lua
  入力をデューティ比で波形を表す矩形波に変換します。
* rc.lua
  入力-抵抗-出力-キャパシタ-GND という回路のシミュレーションを行います。
* singen.lua
  正弦波を生成します。
* test1.lua
  入力をそのまま返します。
* zoufulu.lua
  入力を増幅します。

### 注意点
* 無保証です。万が一このプログラムを利用したことにより直接または間接の損害が発生しても、作者は一切責任を負いません。
* Luaの性質上、処理に長時間かかることがあります。
* 個人で楽しむだけにするなど、音声データの著作権を侵害しないようにしてください。

### 参考サイト
* [The Programming Language Lua](http://www.lua.org/)
* [wav ファイルフォーマット](http://www.kk.iij4u.or.jp/~kondo/wave/)

### Luaのライセンス

> Copyright © 1994–2015 Lua.org, PUC-Rio.
>
> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
>
> The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
