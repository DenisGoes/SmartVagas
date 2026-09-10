package com.estacionamento.api.database;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.SQLException;

public class DatabaseInitializer {



    public void criarTabelaEstacionamento(){
        String sql = """
           CREATE TABLE IF NOT EXISTS estacionamento (
               id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
               nome VARCHAR(100) NOT NULL
           )
           """;

        try(Connection connection = ConnectionFactory.getConnection();
        PreparedStatement stmt = connection.prepareStatement(sql)){

        stmt.execute();
        System.out.println("Tabela estacionamento criada com sucesso!");
    }catch (SQLException e){
            System.out.println("Erro ao criar tabela estacionamento: " + e.getMessage());
        }
    }

    public void criarTabelaVaga(){
        String sql = """
                CREATE TABLE IF NOT EXISTS vaga (
                    id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
                    numero INT NOT NULL,
                    status VARCHAR(50) NOT NULL,
                    estacionamento_id INT NOT NULL,
                    CONSTRAINT fk_estacionamento\s
                    FOREIGN KEY (estacionamento_id)\s
                    REFERENCES estacionamento(id)
                )
                """;
        try (Connection connection = ConnectionFactory.getConnection();
        PreparedStatement stmt = connection.prepareStatement(sql)){
            stmt.execute();
            System.out.println("Tabela vaga criada com sucesso!");
        }catch (SQLException e){
            System.out.println("Erro ao criar tabela vaga: " + e.getMessage());
        }

    }


}
