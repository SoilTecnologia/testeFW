void Bomba() {
  arqNomes = "";
  arqAngulos = "";
  String NOME2[contParam];
  String ANG2[contParam];
  for (int i = 0; i < contParam; i++) {
    if (NOME[i] != "") {
      tamanhoSetor++;
      NOME2[i] = NOME[i];
      arqNomes += NOME2[i];
      arqNomes += ";";
      ANG2[i] = ANG[i];
      arqAngulos += ANG[i];
      arqAngulos += ";";
    }
  }
  arqNomes = arqNomes.substring(0, arqNomes.length() - 1);
  arqAngulos = arqAngulos.substring(0, arqAngulos.length() - 1);
  writeFile(SPIFFS, "/setores.bin", arqNomes.c_str());
  writeFile(SPIFFS, "/angulos.bin", arqAngulos.c_str());
  for (int i = 0; i < tamanhoSetor; i++) {
//    Serial2.println(getValue(readFile(SPIFFS, "/setores.bin"), ';', i));
//    Serial2.println(getValue(readFile(SPIFFS, "/angulos.bin"), ';', i));
  }
}

void LigaBomba() {
  int auxLiga = 0;
  //Serial2.println(tamanhoSetor);
  for (int i = 0; i < tamanhoSetor / 2; i++) {
    if (ligaB[i] < angulo.toInt() + 2 && angulo.toInt() - 2 < desligaB[i]) {
      auxLiga = 1;
      i = tamanhoSetor;
    } else {
      auxLiga = 0;
    }
  }
  if (auxLiga  && digitalRead(BOMBA) == HIGH && (EstadoAtual[0] == '3' || EstadoAtual[0] == '4')) {
    digitalWrite(BOMBA, LOW);
    //Serial2.println("LIGA BOMBA");
  }
  else {
    if (!auxLiga && digitalRead(BOMBA) == LOW) {
      digitalWrite(BOMBA, HIGH);
      //Serial2.println("DESLIGA BOMBA");
    }

  }
}
