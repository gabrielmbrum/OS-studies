barbeiro
- dorme se nao tem cliente
- se tem cliente
	- chama p/ fazer o corte
	- espera cliente sentar
	- faz o corte
	- espera cliente sair
- volta ao inicio

cliente
- acorda o barbeiro (se for o caso)
- espera barbeiro chamar
- segue instruções do barbeiro
- saem após o corte

-------#--------------#---------#-------------#

sit(0)
cut(0)
call_cli(0)
cli_count(0)
leave(0)

barbeiro() {
	while (true) {
		P(cli_count) // faz -1 e espera ate que fique 0
		V(call_cli)
		P(sit)
		V(cut)
		P(leave)
	}
}		

cliente() {
	V(cli_count)
	P(call_cli) //fez -1 e fica de xereca esperando
	V(sit) //adiciono 1 e segue em frente
	P(cut)
	V(leave)
}


