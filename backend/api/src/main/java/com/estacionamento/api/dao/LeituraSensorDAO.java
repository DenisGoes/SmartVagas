package com.estacionamento.api.dao;

import com.estacionamento.api.database.ConnectionFactory;
import com.estacionamento.api.models.LeituraSensor;

import java.sql.*;
import java.time.OffsetDateTime;
import java.util.ArrayList;
import java.util.List;

public class LeituraSensorDAO {

    public boolean cadastrarLeituraSensor(LeituraSensor leituraSensor) {
        String sqlAtualizarVaga = """
                UPDATE vaga
                SET estado = ?
                WHERE id = (
                    SELECT vaga_id
                    FROM sensor
                    WHERE id = ?
                )
                """;

        String sqlInserirLeitura = """
                INSERT INTO leituras_sensor
                (sensor_id, estado)
                VALUES (?, ?)
                """;

        try (Connection connection = ConnectionFactory.getConnection()) {
            connection.setAutoCommit(false);

            try (PreparedStatement atualizarVaga = connection.prepareStatement(sqlAtualizarVaga);
                 PreparedStatement inserirLeitura = connection.prepareStatement(sqlInserirLeitura)) {

                atualizarVaga.setBoolean(1, leituraSensor.getEstado());
                atualizarVaga.setInt(2, leituraSensor.getSensorId());

                int vagasAtualizadas = atualizarVaga.executeUpdate();

                if (vagasAtualizadas == 0) {
                    connection.rollback();
                    System.out.println("Sensor não encontrado: " + leituraSensor.getSensorId());

                    return false;
                }

                inserirLeitura.setInt(1, leituraSensor.getSensorId());
                inserirLeitura.setBoolean(2, leituraSensor.getEstado());

                inserirLeitura.executeUpdate();
                connection.commit();

                return true;
            } catch (SQLException e) {
                connection.rollback();
                throw e;
            }

        } catch (SQLException e) {
            System.out.println("Erro ao cadastrar leitura: " + e.getMessage());
            return false;
        }
    }

    public List<LeituraSensor> listarLeituras() {
        List<LeituraSensor> leituras = new ArrayList<>();

        String sql = """
                SELECT
                    id,
                    sensor_id,
                    estado,
                    data_hora
                FROM leituras_sensor
                ORDER BY data_hora DESC
                """;

        try (Connection connection = ConnectionFactory.getConnection();
             PreparedStatement stmt = connection.prepareStatement(sql);
             ResultSet rs = stmt.executeQuery()) {

            while (rs.next()) {
                LeituraSensor leitura = new LeituraSensor();

                leitura.setId(rs.getInt("id"));
                leitura.setSensorId(rs.getInt("sensor_id"));
                leitura.setEstado(rs.getBoolean("estado"));
                leitura.setDataHora(rs.getObject("data_hora", OffsetDateTime.class));

                leituras.add(leitura);
            }
        } catch (SQLException e) {
            System.out.println("Erro ao listar leituras: " + e.getMessage());
        }
        return leituras;
    }
    public LeituraSensor buscarLeituraPorId(int id) {
        String sql = """
                SELECT
                    id,
                    sensor_id,
                    estado,
                    data_hora
                FROM leituras_sensor
                WHERE id = ?
                """;

        try (Connection connection = ConnectionFactory.getConnection();
             PreparedStatement stmt = connection.prepareStatement(sql)) {

            stmt.setInt(1, id);

            try (ResultSet rs = stmt.executeQuery()) {
                if (rs.next()) {
                    LeituraSensor leitura = new LeituraSensor();

                    leitura.setId(rs.getInt("id"));
                    leitura.setSensorId(rs.getInt("sensor_id"));
                    leitura.setEstado(rs.getBoolean("estado"));
                    leitura.setDataHora(rs.getObject("data_hora", OffsetDateTime.class));

                    return leitura;
                }
            }
        } catch (SQLException e) {
            System.out.println("Erro ao buscar leitura por ID: " + e.getMessage());
        }

        return null;
    }
    public List<LeituraSensor> listarLeiturasPorSensor(int sensorId) {
        List<LeituraSensor> leituras = new ArrayList<>();

        String sql = """
                SELECT
                    id,
                    sensor_id,
                    estado,
                    data_hora
                FROM leituras_sensor
                WHERE sensor_id = ?
                ORDER BY data_hora DESC
                """;

        try (Connection connection = ConnectionFactory.getConnection();
             PreparedStatement stmt = connection.prepareStatement(sql)) {

            stmt.setInt(1, sensorId);

            try (ResultSet rs = stmt.executeQuery()) {
                while (rs.next()) {
                    LeituraSensor leitura = new LeituraSensor();

                    leitura.setId(rs.getInt("id"));
                    leitura.setSensorId(rs.getInt("sensor_id"));
                    leitura.setEstado(rs.getBoolean("estado"));
                    leitura.setDataHora(rs.getObject("data_hora", OffsetDateTime.class));

                    leituras.add(leitura);
                }
            }

        } catch (SQLException e) {
            System.out.println("Erro ao listar leituras do sensor: " + e.getMessage());
        }

        return leituras;
    }

    public boolean deletarLeitura(int id) {
        String sql = """
            DELETE FROM leituras_sensor
            WHERE id = ?
            """;

        try (Connection connection = ConnectionFactory.getConnection();
             PreparedStatement stmt = connection.prepareStatement(sql)) {

            stmt.setInt(1, id);

            int linhasAfetadas = stmt.executeUpdate();

            return linhasAfetadas > 0;

        } catch (SQLException e) {
            System.out.println("Erro ao deletar leitura: " + e.getMessage());
            return false;
        }
    }
    public boolean deletarTodasLeituras() {

        String sql = "DELETE FROM leituras_sensor";

        try (Connection connection = ConnectionFactory.getConnection();
             PreparedStatement stmt = connection.prepareStatement(sql)) {

            stmt.executeUpdate();
            return true;

        } catch (SQLException e) {
            System.out.println("Erro ao deletar todas as leituras: " + e.getMessage());
            return false;
        }
    }
}