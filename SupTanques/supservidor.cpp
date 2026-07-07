#include <iostream>     /* cerr */
#include <algorithm>
#include <cstdint>      /* uint16_t */
#include "supservidor.h"


using namespace std;

/* ========================================
   CLASSE SUPSERVIDOR
   ======================================== */

/// Construtor
SupServidor::SupServidor()
  : Tanks()
  , server_on(false)
  , LU()
  /*ACRESCENTAR*/, thr_server(), sock_con()
{
  // Inicializa a biblioteca de sockets
  /*ACRESCENTAR*/
  mysocket_status iResult = mysocket::init();
  // Em caso de erro, mensagem e encerra
  if (iResult != mysocket_status::SOCK_OK/*MODIFICAR*//*true*/)
  {
    cerr <<  "Biblioteca mysocket nao pode ser inicializada";
    exit(-1);
  }
}

/// Destrutor
SupServidor::~SupServidor()
{
  // Deve parar a thread do servidor
  server_on = false;

  // Fecha todos os sockets dos clientes
  for (auto& U : LU) U.close();
  // Fecha o socket de conexoes
  /*ACRESCENTAR*/
  sock_con.close();
  // Espera o fim da thread do servidor
  /*ACRESCENTAR*/
  if(thr_server.joinable()) thr_server.join();

  // Encerra a biblioteca de sockets
  /*ACRESCENTAR*/
  mysocket::end();
}

/// Liga o servidor
bool SupServidor::setServerOn()
{
  // Se jah estah ligado, nao faz nada
  if (server_on) return true;

  // Liga os tanques
  setTanksOn();

  // Indica que o servidor estah ligado a partir de agora
  server_on = true;

  try
  {
    // Coloca o socket de conexoes em escuta
    /*ACRESCENTAR*/
    mysocket_status iResult = sock_con.listen(SUP_PORT);
    // Em caso de erro, gera excecao
    if (iResult != mysocket_status::SOCK_OK /*MODIFICAR*//*true*/) throw 1;

    // Lanca a thread do servidor que comunica com os clientes
    /*ACRESCENTAR*/
     thr_server = thread([this](){this->thr_server_main();});

    // Em caso de erro, gera excecao
    if (!thr_server.joinable()/*MODIFICAR*//*true*/) throw 2;
  }
  catch(int i)
  {
    cerr << "Erro " << i << " ao iniciar o servidor\n";

    // Deve parar a thread do servidor
    server_on = false;

    // Fecha o socket do servidor
    /*ACRESCENTAR*/
    sock_con.close();

    return false;
  }

  // Tudo OK
  return true;
}

/// Desliga o servidor
void SupServidor::setServerOff()
{
  // Se jah estah desligado, nao faz nada
  if (!server_on) return;

  // Deve parar a thread do servidor
  server_on = false;

  // Fecha todos os sockets dos clientes
  for (auto& U : LU) U.close();
  // Fecha o socket de conexoes
  /*ACRESCENTAR*/
  sock_con.close();
  // Espera pelo fim da thread do servidor
  /*ACRESCENTAR*/
  if(thr_server.joinable()) thr_server.join();
  // Faz o identificador da thread apontar para thread vazia
  /*ACRESCENTAR*/
  thr_server = thread();
  // Desliga os tanques
  setTanksOff();
}

/// Leitura do estado dos tanques
void SupServidor::readStateFromSensors(SupState& S) const
{
  // Estados das valvulas: OPEN, CLOSED
  S.V1 = v1isOpen();
  S.V2 = v2isOpen();
  // Niveis dos tanques: 0 a 65535
  S.H1 = hTank1();
  S.H2 = hTank2();
  // Entrada da bomba: 0 a 65535
  S.PumpInput = pumpInput();
  // Vazao da bomba: 0 a 65535
  S.PumpFlow = pumpFlow();
  // Estah transbordando (true) ou nao (false)
  S.ovfl = isOverflowing();
}

/// Leitura e impressao em console do estado da planta
void SupServidor::readPrintState() const
{
  if (tanksOn())
  {
    SupState S;
    readStateFromSensors(S);
    S.print();
  }
  else
  {
    cout << "Tanques estao desligados!\n";
  }
}

/// Impressao em console dos usuarios do servidor
void SupServidor::printUsers() const
{
  for (const auto& U : LU)
  {
    cout << U.login << '\t'
         << "Admin=" << (U.isAdmin ? "SIM" : "NAO") << '\t'
         << "Conect=" << (U.isConnected() ? "SIM" : "NAO") << '\n';
  }
}

/// Adicionar um novo usuario
bool SupServidor::addUser(const string& Login, const string& Senha,
                             bool Admin)
{
  // Testa os dados do novo usuario
  if (Login.size()<6 || Login.size()>12) return false;
  if (Senha.size()<6 || Senha.size()>12) return false;

  // Testa se jah existe usuario com mesmo login
  auto itr = find(LU.begin(), LU.end(), Login);
  if (itr != LU.end()) return false;

  // Insere
  LU.push_back( User(Login,Senha,Admin) );

  // Insercao OK
  return true;
}

/// Remover um usuario
bool SupServidor::removeUser(const string& Login)
{
  // Testa se existe usuario com esse login
  auto itr = find(LU.begin(), LU.end(), Login);
  if (itr == LU.end()) return false;

  // Remove
  LU.erase(itr);

  // Remocao OK
  return true;
}

/// A thread que implementa o servidor.
/// Comunicacao com os clientes atraves dos sockets.
void SupServidor::thr_server_main(void)
{
  // Fila de sockets para aguardar chegada de dados
  /*ACRESCENTAR*/
  mysocket_queue fila;
  // auxiliares
  mysocket_status iResult;
  uint16_t cmd;
  SupState S;
  uint16_t p;
  tcp_mysocket temp;
  string login, password;
 list<User>::iterator iU;



  while (server_on)
  {
    // Erros mais graves que encerram o servidor
    // Parametro do throw e do catch eh uma const char* = "texto"
    try
    {
      // Encerra se o socket de conexoes estiver fechado
      if (sock_con.closed()/*MODIFICAR*//*true*/)
      {
        throw "socket de conexoes fechado";
      }

      // Inclui na fila de sockets todos os sockets que eu
      // quero monitorar para ver se houve chegada de dados

      // Limpa a fila de sockets
      /*ACRESCENTAR*/
      fila.clear();
      // Inclui na fila o socket de conexoes
      /*ACRESCENTAR*/
      fila.include(sock_con);
      // Inclui na fila todos os sockets dos clientes conectados
      /*ACRESCENTAR*/
      for( auto& U: LU)
        if (U.isConnected()) fila.include(U.sock);

      // Espera ateh que chegue dado em algum socket (com timeout)
      /*ACRESCENTAR*/
     iResult = fila.wait_read(SUP_TIMEOUT*1000);
      // De acordo com o resultado da espera:
      switch(iResult)
      {
          // SOCK_TIMEOUT:
      // Saiu por timeout: nao houve atividade em nenhum socket
      // Aproveita para salvar dados ou entao nao faz nada
      case mysocket_status::SOCK_TIMEOUT:
            // nao faz nada
            break;
            // SOCK_ERROR:
            // Erro no select: encerra o servidor
      case mysocket_status::SOCK_ERROR:
      default: throw "fila de espera";
                break;
      // SOCK_OK:
      case mysocket_status::SOCK_OK:
        // Houve atividade em algum socket da fila:
        //   Testa se houve atividade nos sockets dos clientes.
        try
        {
            for(iU = LU.begin(); server_on && iU!=LU.end(); ++iU)
            {
                //Se sim:
                if(server_on && iU->isConnected() && fila.had_activity(iU->sock))
                {
                    //   - Leh o comando
                   iResult = iU->sock.read_uint16(cmd);
                   if (iResult != mysocket_status::SOCK_OK) throw 1;
                   //   - Executa a acao
                   switch(cmd)
                   {
                        case CMD_LOGIN:
                        case CMD_ADMIN_OK:
                        case CMD_OK:
                        case CMD_ERROR:
                        case CMD_DATA:
                        default:
                        // Para comando invalido
                            throw 2;
                            break;
                        case CMD_GET_DATA:
                            readStateFromSensors(S);
                            iResult = iU->sock.write_uint16(CMD_DATA);
                            if(iResult != mysocket_status::SOCK_OK) throw 3;
                            //escrevendo o parametro do CMD_DATA
                            iResult = iU->sock.write_uint16(S.V1);
                            if (iResult != mysocket_status::SOCK_OK) throw 4;
                            iResult = iU->sock.write_uint16(S.V2);
                            if (iResult != mysocket_status::SOCK_OK) throw 4;
                            iResult = iU->sock.write_uint16(S.H1);
                            if (iResult != mysocket_status::SOCK_OK) throw 4;
                            iResult = iU->sock.write_uint16(S.H2);
                            if(iResult != mysocket_status::SOCK_OK) throw 4;
                            iResult = iU->sock.write_uint16(S.PumpInput);
                            if (iResult != mysocket_status::SOCK_OK) throw 4;
                            iResult = iU->sock.write_uint16(S.PumpFlow);
                            if (iResult != mysocket_status::SOCK_OK) throw 4;
                            iResult = iU->sock.write_uint16(S.ovfl);
                            if (iResult != mysocket_status::SOCK_OK) throw 4;

                            break;

                       case CMD_SET_V1:
                           //conferindo se eh admin
                           if(!(iU->isAdmin)) throw 5;
                           // lendo o parametro de CMD_SET_V1
                           iResult = iU->sock.read_uint16(p, SUP_TIMEOUT*1000);
                           if (iResult != mysocket_status::SOCK_OK) throw 6;
                           setV1Open(static_cast<bool>(p));
                           // imprimindo uma mensagem de mudanca na valvula 1
                           cout << "CMD_SET_V1 " << p << " de " << iU->login << endl;
                            iResult = iU->sock.write_uint16(CMD_OK);
                            if (iResult != mysocket_status::SOCK_OK) throw 7;
                            break;
                       case CMD_SET_V2:
                            //conferindo se eh admin
                            if(!(iU->isAdmin)) throw 8;
                            //lendo o paramtetro de CMD_SET_V2
                            iResult = iU->sock.read_uint16(p, SUP_TIMEOUT*1000);
                            if (iResult != mysocket_status::SOCK_OK) throw 9;
                            setV2Open(static_cast<bool>(p));
                            //imprimindo uma mensagem de mudanca na valvula 2
                            cout << "CMD_SET_V2 " << p << " de " << iU->login << endl;
                            iResult = iU->sock.write_uint16(CMD_OK);
                            if (iResult != mysocket_status::SOCK_OK) throw 10;
                            break;
                      case CMD_SET_PUMP:
                            //conferindo se eh admin
                            if(!(iU->isAdmin)) throw 11;
                            //lendo o paramtetro de CMD_SET_PUMP
                            iResult = iU->sock.read_uint16(p, SUP_TIMEOUT*1000);
                            if (iResult != mysocket_status::SOCK_OK) throw 12;
                            setPumpInput(p);
                            // imprimindo uma mensagem de debug
                            cout << "CMD_SET_PUMP " << p << " de " << iU->login << endl;
                            iResult = iU->sock.write_uint16(CMD_OK);
                            if (iResult != mysocket_status::SOCK_OK) throw 13;
                            break;
                      case CMD_LOGOUT:
                        //Fechando o socket
                        iU->close();
                        // imprimindo uma mensagem de cliente deslogado
                        cout << "CMD_LOGOUT " << iU->login << endl;
                        break;

                   }

                }

            }
        }
        catch(int i)
        {
            switch(i)
            {
                case 5: //nao eh admin
            iU->sock.write_uint16(CMD_ERROR);
            // imprimindo uma mensagem de debug
            cerr << "CMD_SET_V1 " << iU->login << " (ERRO)" << endl;
            break;
          case 8: //nao eh admin
            iU->sock.write_uint16(CMD_ERROR);
            //o servidor imprimindo uma mensagem de debug
            cerr << "CMD_SET_V2 " << iU->login << " (ERRO)" << endl;
            break;
          case 11: // nao eh admin
            iU->sock.write_uint16(CMD_ERROR);
            // imprimindo uma mensagem de debug
            cerr << "CMD_SET_PUMP " << iU->login << " (ERRO)" << endl;
            break;
          case 1: //erro de leitura no comando
          case 2: //comando invalido
          case 3: //erro de escrita
          case 4: //erro de escrita
          case 6: //erro de leitura
          case 7: //erro de escrita
          case 9: //erro de leitura
          case 10: //erro de escrita
          case 12: //erro de leitura
          case 13: //erro de escrita
          default:
            //Fechando o socket
            iU->close();
            // Informando o erro na comunicacao ou comando invalido
            cerr << "Erro " << i << " na comunicacao ou comando invalido do cliente "<< iU->login << endl;
            break;
            }
        }
       //   Depois, testa se houve atividade no socket de conexao
       if (server_on && sock_con.connected() && fila.had_activity(sock_con))
        {

          iResult = sock_con.accept(temp);
          if (iResult != mysocket_status::SOCK_OK) throw "aceitar provisoriamente a conexao";

          try
          {
            // Lendo o comando
            iResult = temp.read_uint16(cmd, SUP_TIMEOUT*1000);
            if (iResult != mysocket_status::SOCK_OK) throw 1;

            // Testando o comando
            if (cmd!=CMD_LOGIN) throw 2;

            // Lendo o login do usuario que deseja fazer conexao
            iResult = temp.read_string(login, SUP_TIMEOUT*1000);
            if (iResult != mysocket_status::SOCK_OK) throw 3;

            // Lendo o password do usuario que deseja fazer conexao
            iResult = temp.read_string(password, SUP_TIMEOUT*1000);
            if (iResult != mysocket_status::SOCK_OK) throw 4;

            // Testando os dados do novo usuario
            if (login.size()<6 || login.size()>12) throw 5;
            if (password.size()<6 || password.size()>12) throw 5;

            // Verificando se jah existe um usuario cadastrado com esse login
            iU = find(LU.begin(), LU.end(), login);
            if(iU==LU.end()) throw 6; // Erro se nao existir

            // Testando se a senha confere
            if (iU->password != password) throw 7; // Senha nao confere

            // Testando se o cliente jah estah conectado
            if (iU->isConnected()) throw 8; // User jah conectado

            // Associando o socket que se conectou a um usuario cadastrado
            iU->sock.swap(temp);

            // Enviando a confirmacao de conexao para o novo cliente
            iResult = (iU->isAdmin ? iU->sock.write_uint16(CMD_ADMIN_OK) : iU->sock.write_uint16(CMD_OK));
            if (iResult != mysocket_status::SOCK_OK) throw 9;

            //imprimindo  uma mensagem de cliente conectado
            cout << "CMD_LOGIN " << iU->login << " (CONECTADO)" << endl;
          }
          catch (int i)
          {
            if (i>=5 && i<=8)
            {
              //  login invalido
              // Enviando comando informando login invalido
              temp.write_uint16(CMD_ERROR);
              // Esperando 1 segundo antes de fechar o socket
              this_thread::sleep_for(chrono::seconds(1));
              // Erro na comunicacao com socket temporario
              temp.close();
              //o servidor imprimindo uma mensagem de debug do cliente
              cerr << "CMD_LOGIN " << login << " (ERRO)" << endl;
            }
            else
            {
              if(i==9)
              {
                // Erro na comunicacao com socket
                iU->close();
              }
              else
              {
                // Erro na comunicacao com socket temporario
                temp.close();
              }
              // Informa erro nao previsto
              cerr << "Erro " << i << " na conexao do cliente" << endl;
            }
          }
        }
        break;
      }
    }
      // SOCK_TIMEOUT:
      // Saiu por timeout: nao houve atividade em nenhum socket
      // Aproveita para salvar dados ou entao nao faz nada
      // SOCK_ERROR:
      // Erro no select: encerra o servidor
      // SOCK_OK:
      // Houve atividade em algum socket da fila:
      //   Testa se houve atividade nos sockets dos clientes. Se sim:
      //   - Leh o comando
      //   - Executa a acao
      //   = Envia resposta
      //   Depois, testa se houve atividade no socket de conexao. Se sim:
      //   - Estabelece nova conexao em socket temporario
      //   - Leh comando, login e senha
      //   - Testa usuario
      //   - Se deu tudo certo, faz o socket temporario ser o novo socket
      //     do cliente e envia confirmacao

     // fim try - Erros mais graves que encerram o servidor
    catch(const char* err)  // Erros mais graves que encerram o servidor
    {
      cerr << "Erro no servidor: " << err << endl;

      // Sai do while e encerra a thread
      server_on = false;

      // Fecha todos os sockets dos clientes
      for (auto& U : LU) U.close();
      // Fecha o socket de conexoes
      /*ACRESCENTAR*/
      sock_con.close();
      // Os tanques continuam funcionando

    } // fim catch - Erros mais graves que encerram o servidor
  } // fim while (server_on)
}



