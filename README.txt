Para compilar o programa em Visual Studio com a biblioteca voce:

-As pastas grammar e lib devem estar no diretorio do source
-As "Include Directories" do projeto devem contêr os caminhos para a pasta include do Java e dentro dela a pasta win32.
	Usualmente estes estão em C:\Program Files (x86)\Java\jdk1.x.x_xx\include e C:\Program Files (x86)\Java\jdk1.x.x_xx\include\win32
-As "Library Directories" devem conter o caminho para a pasta lib do Java (usualmente C:\Program Files (x86)\Java\jdk1.x.x_xx\lib)
-A variável de ambiente PATH deve contêr o caminho para o ficheiro jvm.dll (usualmente C:\Program Files (x86)\Java\jre7\bin\client)
-As "Additional Dependencies" devem conter o ficheiro jvm.lib