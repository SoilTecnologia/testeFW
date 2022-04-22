

void showFile()
{
  StaticJsonDocument<2048> dochist;
  JsonArray intencaoJSON = dochist.createNestedArray("Intencao");
  JsonArray tsJSON = dochist.createNestedArray("Timestamp");
  dados = "";
  int count = 0;
  String linha = "";
  String linhastr = "";
  int tamanho = 0;
  registros = 0;
  // Exibe na Serial2.e no display o início do arquivo
  //Serial2.println("# Begin of file 1 #");
  errorMsg = "";

  // Posiciona o ponteiro do arquivo no início
  ObjFS.rewind();

  // Exibe todos os registros até o fim
  while (ObjFS.readFileNextRecord(&linha, &errorMsg) && linha != "")
  {
    dados += linha;
    intencaoJSON.add(linha.substring(linha.indexOf(":") + 1, linha.indexOf("/")));
    tsJSON.add(linha.substring(linha.indexOf("/") + 1, linha.indexOf(" ")));
    //Serial2.println(linhastr);
    tamanho += linhastr.length();
    //  Serial2.println(linhastr.length());
    Serial2.println(linha);
    count++;

  }

  // Se existir mensagem de erro exibe na Serial2.e no display
  if (errorMsg != "")
  {
    //Serial2.println(errorMsg);
  }

  // Exibe na Serial2.e no display o fim do arquivo
  //Serial2.println("# End of file 1#");
  registros = count;
  //DynamicJsonDocument<tamanho> dochist;
  //String response;
  int b = serializeJson(dochist, responseHist);
  //request->send(200, "application/json", response);
  //Serial2.println(b);
}


void showFile2()
{
  dados = "";
  int count = 0;
  String linha = "";
  registros2 = 0;
  // Exibe na Serial2.e no display o início do arquivo
  //Serial2.println("# Begin of file 2 #");
  errorMsg = "";

  // Posiciona o ponteiro do arquivo no início
  AgFS.rewind();

  // Exibe todos os registros até o fim
  while (AgFS.readFileNextRecord(&linha, &errorMsg) && linha != "")
  {
    dados += linha;
    //Serial2.println(linha);
    count++;

  }

  // Se existir mensagem de erro exibe na Serial2.e no display
  if (errorMsg != "")
  {
    //Serial2.println(errorMsg);
  }

  // Exibe na Serial2.e no display o fim do arquivo
  //Serial2.println("# End of file 2#");
  registros2 = count;
}


void showFile3()
{
  dados = "";
  int count = 0;
  String linha = "";
  registros3 = 0;
  // Exibe na Serial2.e no display o início do arquivo
  Serial2.println("# Begin of file 3 #");
  errorMsg = "";

  // Posiciona o ponteiro do arquivo no início
  PosFS.rewind();

  // Exibe todos os registros até o fim
  while (PosFS.readFileNextRecord(&linha, &errorMsg) && linha != "")
  {
    dados += linha;
    Serial2.println(linha);
    count++;

  }

  // Se existir mensagem de erro exibe na Serial2.e no display
  if (errorMsg != "")
  {
    Serial2.println(errorMsg);
  }

  // Exibe na Serial2.e no display o fim do arquivo
  Serial2.println("# End of file 3#");
  registros3 = count;
}
