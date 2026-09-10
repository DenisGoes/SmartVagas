package com.estacionamento.api.database;

import java.sql.Connection;
import java.sql.Driver;
import java.sql.DriverManager;
import java.sql.SQLException;

public class ConnectionFactory {
    private static final String USER = System.getenv("POSTGRES_USER");
    private static final String PASS = System.getenv("POSTGRES_PASSWORD");
    private static final String PORT = System.getenv("POSTGRES_PORT");
    private static final String DB = System.getenv("POSTGRES_DB");

    private static final String URL = "jdbc:postgresql://localhost:" + PORT + "/" + DB;

    public static Connection getConnection() throws SQLException{
        Connection connection = DriverManager.getConnection(URL, USER, PASS);
        System.out.println("Conexão realizada com sucesso!");
        return connection;
    }




}
