package com.estacionamento.api.database;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

public class ConnectionFactory {

    private static final String URL = System.getenv("DATABASE_URL");
    private static final String USER = System.getenv("DATABASE_USER");
    private static final String PASSWORD = System.getenv("DATABASE_PASSWORD");

    public static Connection getConnection() throws SQLException {

        if (URL == null || URL.isBlank()) {
            throw new SQLException("DATABASE_URL não configurada.");
        }

        if (USER == null || USER.isBlank()) {
            throw new SQLException("DATABASE_USER não configurada.");
        }

        if (PASSWORD == null || PASSWORD.isBlank()) {
            throw new SQLException("DATABASE_PASSWORD não configurada.");
        }

        Connection connection = DriverManager.getConnection(
                URL,
                USER,
                PASSWORD
        );

        try (var stmt = connection.createStatement()) {
            stmt.execute("SET TIME ZONE 'America/Sao_Paulo'");
        }

        System.out.println("Conexão com Neon realizada com sucesso!");

        return connection;
    }
}