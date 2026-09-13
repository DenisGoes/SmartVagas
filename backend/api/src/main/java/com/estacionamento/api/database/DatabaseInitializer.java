package com.estacionamento.api.database;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.SQLException;

public class DatabaseInitializer {

    public void criarTabelaEstacionamento() {
        String sql = """
                CREATE TABLE IF NOT EXISTS estacionamento (
                    id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
                    nome VARCHAR(100) NOT NULL
                )
                """;
        executar(sql, "estacionamento");
    }

    public void criarTabelaVaga() {
        String sql = """
                CREATE TABLE IF NOT EXISTS vaga (
                    id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
                    numero VARCHAR(20) NOT NULL,
                    estacionamento_id INT NOT NULL,
                    estado BOOLEAN NOT NULL DEFAULT FALSE,

                    CONSTRAINT fk_vaga_estacionamento
                    FOREIGN KEY (estacionamento_id)
                    REFERENCES estacionamento(id)
                )
                """;
        executar(sql, "vaga");
    }

    public void criarTabelaSensor() {
        String sql = """
                CREATE TABLE IF NOT EXISTS sensor (
                    id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
                    nome VARCHAR(100) NOT NULL,
                    vaga_id INT NOT NULL,

                    CONSTRAINT fk_sensor_vaga
                    FOREIGN KEY (vaga_id)
                    REFERENCES vaga(id)
                )
                """;
        executar(sql, "sensor");
    }

    public void criarTabelaLeiturasSensor() {
        String sql = """
                CREATE TABLE IF NOT EXISTS leituras_sensor (
                    id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
                    sensor_id INT NOT NULL,
                    estado BOOLEAN NOT NULL,
                    data_hora TIMESTAMP WITH TIME ZONE
                        NOT NULL DEFAULT CURRENT_TIMESTAMP,

                    CONSTRAINT fk_leitura_sensor
                    FOREIGN KEY (sensor_id)
                    REFERENCES sensor(id)
                )
                """;
        executar(sql, "leituras_sensor");
    }

    private void executar(String sql, String nomeTabela) {
        try (Connection connection = ConnectionFactory.getConnection();
             PreparedStatement stmt = connection.prepareStatement(sql)) {

            stmt.execute();
            System.out.println("Tabela " + nomeTabela + " criada com sucesso!");
        } catch (SQLException e) {
            System.out.println("Erro ao criar tabela " + nomeTabela + ": " + e.getMessage());
        }
    }
}