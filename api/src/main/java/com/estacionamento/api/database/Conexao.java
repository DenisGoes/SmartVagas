package com.estacionamento.api.database;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.SQLException;

public class Conexao {

    public static Connection conectar(){
        String db = System.getenv("POSTGRES_DB");
        String usuario = System.getenv("POSTGRES_USER");
        String senha = System.getenv("POSTGRES_PASSWORD");
        String porta = System.getenv("POSTGRES_PORT");

        String url = "jdbc:postgresql://localhost:" + porta + "/" + db;

        try{
            Connection conexao = DriverManager.getConnection(url, usuario, senha);
            System.out.println("Conectado com sucesso!");

            return conexao;

        } catch(SQLException e){
            System.out.println("Erro encontrado!" + e.getMessage());

            return null;
        }
    }
}
