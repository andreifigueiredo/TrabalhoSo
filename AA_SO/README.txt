Para executar mymallocteste: ./mymallocteste --thread=t --size=s --count=c --loop=l
Para executar mymallocteste: ./mallocteste --thread=t --size=s --count=c --loop=l
(substituir as letras pelas entradas abaixo.
Os testes que devem se realizados tiveram as seguntes entradas:
--thread=2 --size=5 --count=5 --loop=5
--thread=4 --size=10 --count=10 --loop=10
--thread=8 --size=20 --count=20 --loop=10

O myMalloc utiliza o conceito de blocos e lista de memória, foi levado em consideração o uso de mapa de memória, mas levando em conta que o mapa de memória não tem a possibilidade de aumentar seu tamanho durante a execução, ele foi descartado.
O algoritmo para decidir que bloco usar é o FIRST FIT por ser o mais facil de se implementar e o mais rápido, aconteceu a tentativa de usar o BEST FIT, mas resultou em várias falhas e devido ao tempo restante foi necessário usar o FIRST FIT. 
Uma das pricipais vantagens do mymalloc é que ele é threadsafe. Outra idéia que o mymalloc usa é criar duas listas, uma lista será responsável pela memória solicitada pelo sbrk() e outra lista pela memória livre. 
Para agilizar o percorrimento pela freelist, sempre quando um bloco é adicionado a freeLista ele é ordenado, visto que sempre que é preciso alocar memória é necessário percorrer a freelist. 
Para aproveitar o máximo de memória que é requisitada pela função sbrk(), sempre quando possivel a memória que sobra de uma alocação é acrescentada a outro bloco livre pela função verifyFusion(), com isso é minimizada a memória ociosa.
Para mostrar a fragmentação de memória foi criada a função  show_alloc_mem(), que não está funcionando, ela simplesmente percorre a memoryList e imprime o ponteiro, o offset, e o tamanho do bloco, dos blocos alocados.

verifyFusion: Ele ocorre em duas situações, caso seja alocada uma memória e sobre espaço no bloco que foi alocada, ou quando adicionamos um bloco ao freeList.É usada duas formulas para realizar essa fusão ambas fudem um bloco ao outro, mas cada uma tem uma formula diferente:
sizeDoBloco = size requisitado pelo usário que o bloco deve ter
tamanhoDoBloco = tamanho que a struct ocupa na memoria
1.(begin && begin->prox) = endereço + sizeDoBlocoBegin + tamanhoDoBlocoBegin;
2.(block->magic == FREE_MAGIC) = sizeDoBloco + sizeDoBlocoBlock + tamanhoDoBlocoBlock;
Na situação onde (block->magic == FREE_MAGIC) o bloco que vem em seguida não esta livre então é necessário tomar cuidado com os ponteiros.

freeList: Lista com todos os blocos livres.
memoryList: Lista com todos os blocos.

Thread Safety: é um conceito de programação de computadores aplicável no contexto de programas multi-thread. Um pedaço de código é dito thread-safe se ele apenas manipula estruturas de dados compartilhadas de uma forma que garanta uma execução segura através de várias threads ao mesmo tempo

O mymallocteste foi pensado em explorar ao maximo as vatagens do mymalloc,  ele é um programa multithread para aproveitar o threadsafety do mymalloc. Ele aloca muita memória para mostrar a vantagem de se usar um lista de memória e ultiliza a biblioteca App.h para calcular o tempo de parede.

Conclusão: É possivel diminuir o tempo de alocação com estratégias basicas de gerencia de memória, entretanto a aplicação dessa estratégias pode ser bem dificil, pois se deve ter muito cuidado com a memória e os ponteiros. Utilizando uma estrutura simples de lista de memória e algumas estratégias de gerenciamento de memória o mymalloc conseguiu ganhar do malloc em todos os testes, existe uma grande possibilidade de caso seja testado acesso a memória o mymalloc também seria superior ao malloc. Uma das melhoras que poderiam ser implementadas seriam o uso de BET FIT ou WORST FIT, para diminuir a fragmentação e o uso da estrutura da hashtable para melhorar o acesso a memória.

Bibliografias:  https://pt.wikipedia.org/wiki/Thread_safety
		http://stackoverflow.com/questions/10706466/how-does-malloc-work-in-a-multithreaded-environment
		http://stackoverflow.com/questions/9989496/malloc-and-freeing-memory-between-threads-in-c
		http://www.inf.puc-rio.br/~inf1007/material/slides/alocacaodinamica.pdf
		http://tldp.org/LDP/tlk/mm/memory.html
	Livro: The Linux Programming Interface
