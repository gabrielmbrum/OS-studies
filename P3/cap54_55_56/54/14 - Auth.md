Nos referimos ao requerente ou usuário no contexto da autenticação como `agente`. 
Toda estrutura de dados usada para o gerenciamento de permissões e decisões como esta são chamados de `credenciais`.
Tais credencias dão informações importante para que o SO decida se o agente pode ou não acessar um `objeto` da requisição.

Empiricamente, sabemos que usuários e programas tem privilégios de acesso a objetos distintos e o escopo deste acesso tem que ser gerenciado pelo SO. Para endereçar esse problema, podemos escrever cada processo com sua identidade própria, associado ao ID do usuário também.

Entretanto, como podemos autenticar o usuário? "confirmar que ele é quem diz ser"

# Autenticação de senha
Autenticação baseada no que se sabe, ou seja, apenas usar uma senha (algo que o usuário sabe) -> criptografia: precisamos apenas salvar o hash referente a senha, nao a senha em si 

Criptografias possiveis são parte dos hashes criptográficos, em especial, hoje, o melhor que se tem gira em torno SHA-256 e SHA-3.

Uma forma de se 'adivinhar' senhas é chamado de `ataque de dicionário`, ou seja, palavras em comum com a experiência empírica do individuo que a mantém.

Para evitar o ataque de dicionário até em arquivos que contenham unicamente a senha hasheada, podemos usar o `salt`. Essa técnica consiste em adicionar um numero aleatório com tamanho entre 32 e 64 bits (ou maior) e concatená-lo na senha não hasheada. Assim, precisamos guardar esse número, para concatenar em todas as tentativas de login. Essa técnica impede o chamado `rainbow tables` , que foi explicado acima.

Existem técnicas como `multi-factor auth` (em especial `two-factor auth`) que podem ser futuramente exploradas com mais calma (não vou escrever sobre).

# Autenticação por tokens externos

podemos usar dongles com tokens de seguranca tambem criptografados, ou ate pelo celular.

# Autenticação biométrica

Biometria: 
	Ex: impressão digital, reconhecimento facial, padrões de digitação.

# Usuários "Não-Humanos"
A exemplo de algum processo como 'docker', algum deamon ou enfim, qualquer user nao humano. É importante que o usuário que criou o processo não humano tenha credenciais superiores ao deste, assim ele pode criar um processo "subserviente" aos seus privilégios. Isso nos leva a ideia de grupos de usuários com permissões compartilhadas entre outros.