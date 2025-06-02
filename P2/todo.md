## Chapters

- [x] 31
- [x] 32
- [x] 35
- [x] 36
- [x] 37
- [x] 38 (16 pgs)
- [x] 44 (18 pgs)

---

### Assuntos que caíram em provas anteriores

1. Deadlock (explicação de como pode entrar em deadlock, suas condições de existência e como evitar)

2. Algoritmo de escalonamento de disco

3. vantagem de acumular operações de trilhas próximas para serem feitas de uma vez

4. vantagem de acumular operações de escrita em um SSD para executar tudo de uma vez

5. algoritmo do elevador para SSD 

6. como modificar o RAID1 para ter stripping e qual sua vantagem?

7. problema do barbeiro dorminhoco com dois barbeiros

8. jantar dos filósofos em que cada filosofo pega dois hashis de uma vez e tenha um garçom

----

### Repostas das questões

1. Pode-se entrar em deadlock por meio de um bloqueamento múto de avanço, onde uma thread A espera pelo lock adquirido pela B e a thread B espera pelo lock adquirido pela thread A.
    As condições para se entrar em deadlock é: exclusão mútua, não ter preempção, "hold-and-wait" e lista circular.
    Para evitar, utiliza-se respectivamente: compareAndSwap, trylock(), adquire TODOS os locks de uma só vez e adquire os locks SEMPRE na mesma ordem.

6. é igual a imagem de RAID 1 do resumo. ajuda na leitura paralela.
