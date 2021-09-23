
## Conversão entre codificações UTF-8 e  _varint_

O objetivo deste trabalho é implementar, na linguagem C, duas funções (**`utf_varint`**  e  **`varint_utf`**), que convertem, respectivamente, arquivos em formato  **UTF-8**  para o formato  **varint**  e arquivos em formato  **varint**  para o formato  **UTF-8**  
  

## Codificação Unicode

Em computação, caracteres de texto são tipicamente representados por códigos especificados por algum padrão de codificação. Um padrão bastante conhecido é a  codificação ASCII que utiliza valores inteiros de 0 a 127 para representar letras, dígitos e alguns outros símbolos. Algumas extensões dessa codificação utilizam também a faixa de valores de 128 a 255 para representar, por exemplo, caracteres acentuados e alguns outros símbolos adicionais.

A codificação ASCII e outros padrões de codificação que se limitem a um único byte para representar caracteres ficam restritos a apenas 256 símbolos diferentes. A codificação Unicode foi criada, no final da década de 1980, para permitir a representação de um conjunto maior de símbolos. A versão corrente dessa codificação é capaz de representar os caracteres utilizados por todos os idiomas conhecidos, além de diversos outros símbolos.

Cada caractere em Unicode é associado a um código (_code point_) na faixa de 0 a 0x10FFFF, o que, com a exclusão de alguns códigos não utilizados, permite a representação de 1.111.998 símbolos diferentes. Na notação adotada pelo padrão Unicode,  **U+xxxx**  identifica o código com valor hexadecimal xxxx. Por exemplo, o código U+00A9 (o código do símbolo ©) corresponde ao valor hexadecimal 0x00A9.

Uma faixa de valores (o intervalo 0xD800 a 0xDFFF) não deve ser utilizada para a representação de caracteres ou símbolos. Essa faixa é reservada para uso como  _surrogates_  na codificação UTF-16, permitindo a representação de códigos superiores a 0xFFFF. Contudo, na prática essa regra é muitas vezes ignorada em outras codificações UTF, que não UTF-16.

_Codepoints_  Unicode podem ser representados através de diferentes tipos de codificação de caracteres. O padrão Unicode define as codificações UTF-8, UTF-16 e UTF-32. Para este trabalho, a codificação de interesse é a  **UTF-8**. Essa codificação é a mais utilizada atualmente em sistemas Linux, e é também a codificação dominante na Web.  
  

### Codificação UTF-8

Na codificação UTF-8, os códigos dos caracteres são representados em um número variável de bytes. O tamanho mínimo utilizado para representar um caractere em UTF-8 é um byte (8 bits); se a representação necessita de mais espaço, mais bytes são utilizados (até o máximo de 4 bytes).

Uma característica importante é que a codificação UTF-8 é compatível com o padrão ASCII, ou seja, os 128 caracteres associados aos códigos de 0 a 0x7F em ASCII tem a mesma representação (em um único byte) em UTF-8.

A tabela a seguir indica para cada faixa de valores de códigos Unicode o número de bytes necessários para representá-los e a codificação usada para essa faixa.

| Código Unicode      | Representação UTF-8 (byte a byte) |
| ----------- | ----------- |
| U+0000 a U+007F      | 0xxxxxxx      |
|U+0080 a U+07FF  | 110xxxxx 10xxxxxx       |
| U+0800 a U+FFFF  | 1110xxxx 10xxxxxx 10xxxxxx        |
| U+10000 a U+10FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx        |
|


Note que:

-   x é o valor de um bit. O bit x da extrema direita é o bit menos significativo.
-   Um código representado em um apenas 7 bits tem sempre 0 no bit mais significativo.
-   Se um código é representado por uma sequência de bytes, o  **número de 1s**  no início do primeiro byte indica o número total de bytes da sequência. Esses  **1**s são seguidos sempre por um  **0**.
-   Todos os bytes que se seguem ao primeiro começam com  **10**  (indicando que é um byte de continuação).

**Exemplos:**

1.  O símbolo © tem código Unicode U+00A9.  
    Em binário A9 é  **1010 1001**. Usando a codificação de 2 bytes para a faixa U+0080 a U+07FF temos:
    
    110**00010**  10**101001**  = 0xC2 0xA9
    
    O primeiro byte começa com 110, indicando que a sequência é composta por dois bytes. A seguir vêm os cinco primeiros bits do código Unicode (note o preenchimento com zeros à esquerda para completar a porção do código do caractere colocada no primeiro byte da sequência).
    
    O segundo byte começa com 10, indicando que é um byte de continuação. A seguir vêm os próximos seis bits do código Unicode.
    
2.  O símbolo ≠ tem código Unicode U+2260.  
    Em binário 2260 é  **0010 0010 0110 0000**. Usando a codificação de 3 bytes para a faixa U+0800 a U+FFFF temos:
    
    1110**0010**  10**001001**  10**100000**  = 0xE2 0x89 0xA0
    
    O primeiro byte começa com 1110, indicando que a sequência é composta por três bytes. A seguir vêm os quatro primeiros bits do código Unicode
    
    O segundo e o terceiro bytes começam com 10, indicando que são bytes de continuação. Cada um deles tem, em seguida, os próximos seis bits do código Unicode.
    

 
## Codificação  _varint_

_Varint_  é um método de serialização de inteiros que, assim como UTF-8, utiliza um número variável de bytes, de forma que quanto menor o valor a ser codificado, menor o número de bytes. Esse tipo de codificação é utilizado, por exemplo, no padrão  [Protocol Buffers](https://developers.google.com/protocol-buffers/docs/overview), um mecanismo de serialização de dados definido pela Google para uso em protocolos de comunicação e armazenamento de dados.

Para fazer a codificação  _varint_  de um valor inteiro, o valor original é dividido em grupos de 7 bits,  **começando com os bits menos significativos**. Cada um desses grupos gera um byte da codificação; o bit mais significativo de cada byte gerado indica se ele é o último byte do valor codificado (0) ou se há mais bytes em seguida (1).

Vejamos um exemplo, a codificação do inteiro 300 como um  _varint_. Em binário, temos:

00000000 00000000 00000001 00**101100**

Pegamos o primeiro grupo de 7 bits (os menos significativos, em azul), e acrescentamos um oitavo bit para indicar que haverá mais bytes a seguir:

 **10101100**

  
Este é o primeiro byte da codificação. Pegamos agora o próximo grupo de 7 bits. Como os demais grupos contém apenas zeros, este será o último byte do valor codificado:

 **00000010**

  
A codificação _varint_ do valor inteiro 300 utiliza então dois bytes:

**10101100 00000010**

que corresponde, em hexadecimal, à sequência 0xAC 0x02.

 

## Funções de Conversão

O objetivo deste trabalho é implementar, na linguagem C, as funções  **`utf_varint`**  e  **`varint_utf`**, que realizam respectivamente, a conversão de arquivos em formato  **UTF-8**  para o formato  **varint**  e arquivos em formato  **varint**  para o formato  **UTF-8**.  
  

### Conversão UTF-8 para varint

A função **`utf_varint`** deve ler o conteúdo de um arquivo de entrada codificado em UTF-8, obter os valores dos _codepoints_ representados, e gravar em um arquivo de saída esses valores codificados como _varints_. O protótipo (cabeçalho) dessa função é o seguinte:

 ```int utf_varint(FILE *arq_entrada, FILE *arq_saida); ```

Os dois parâmetros da função são dois arquivos abertos em modo binário: o arquivo de entrada (`arq_entrada`) e o arquivo de saída (`arq_saida`).

O valor de retorno da função  `**utf8_var**`  é 0, em caso de sucesso e -1, em caso de erro de E/S. Em caso de erro, a função deve emitir, na saída de erro (stderr), uma mensagem indicando qual o tipo de erro ocorrido (leitura ou gravação) e retornar imediatamente.

Por simplicidade, você pode considerar que o arquivo de entrada sempre estará  CORRETAMENTE CODIFICADO. Dessa forma, você não precisa implementar o tratamento de erros de codificação do arquivo de entrada, apenas erros de E/S.

  

### Conversão varint para UTF-8

A função **`varint_utf`** deve ler o conteúdo de um arquivo de entrada contendo valores inteiros codificados como _varints_, obter os valores inteiros originais, e gravar em um arquivo de saída os valores obtidos, codificados em UTF-8.

O protótipo da função é o seguinte:

 **int varint_utf(FILE *arq_entrada, FILE *arq_saida);** 

Novamente, os parâmetros da função são dois arquivos abertos em modo binário: o arquivo de entrada (`arq_entrada`) e o arquivo de saída (`arq_saida`).

Assim como na função anterior, o valor de retorno é 0, em caso de sucesso e -1, em caso de erro de E/S. Em caso de erro, a função deve retornar após emitir, na saída de erro (stderr) uma mensagem indicando o tipo de erro ocorrido (leitura ou gravação).

Por simplicidade, você pode considerar que o arquivo de entrada sempre estará  CORRETAMENTE CODIFICADO, ou seja, todos os valores inteiros codificados no arquivo correspondem a  _codepoints_  válidos.


## Dicas

Para verificar o conteúdo do arquivo gravado, você pode usar o utilitário  **`hexdump`**. Por exemplo, o comando

hexdump -C < nome-do-arquivo > 

exibe o conteúdo do arquivo especificado byte a byte, em hexadecimal (16 bytes por linha). A segunda coluna de cada linha (entre '|') exibe os caracteres ASCII correspondentes a esses bytes, **se eles existirem**. Experimente inspecionar os arquivos dados como exemplo com o hexdump!

Para abrir um arquivo para gravação ou leitura em formato binário, use a função

FILE *fopen(char *path, char *mode); 

descrita em `stdio.h`. Seus argumentos são:

-   `path`: nome do arquivo a ser aberto
-   `mode`: uma string que, no nosso caso, será  **"rb"**  para abrir o arquivo para leitura em modo binário ou  **"wb"**  para abrir o arquivo para escrita em modo binário.

A letra 'b', que indica o modo binário, é ignorada em sistemas como Linux, que tratam da mesma forma arquivos de tipos texto e binário. Mas ela é necessária em outros sistemas, como Windows, que tratam de forma diferente arquivos de tipos texto e binário (interpretando/modificando, por exemplo, bytes com códigos que correspondem a caracteres de controle, como quebra de linha).

Para fazer a leitura e gravação do arquivo, pesquise as funções  `fwrite`/`fread`  e/ou  `fputc`/`fgetc`.  
