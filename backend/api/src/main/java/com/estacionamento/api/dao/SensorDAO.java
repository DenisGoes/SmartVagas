package com.estacionamento.api.dao;

import com.estacionamento.api.database.ConnectionFactory;
import com.estacionamento.api.models.Sensor;

import java.sql.*;
import java.util.ArrayList;
import java.util.List;

public class SensorDAO {

    public void cadastrarSensor(Sensor sensor) {
        String sql = "INSERT INTO sensor (nome, vaga_id) VALUES (?, ?)";

        try (Connection connection = ConnectionFactory.getConnection();
             PreparedStatement stmt = connection.prepareStatement(sql)) {

            stmt.setString(1, sensor.getNome());
            stmt.setInt(2, sensor.getVagaId());

            stmt.executeUpdate();
        } catch (SQLException e) {
            System.out.println("Erro ao cadastrar sensor: " + e.getMessage());
        }
    }

    public List<Sensor> listarSensores() {
        List<Sensor> sensores = new ArrayList<>();

        String sql = "SELECT * FROM sensor";

        try (Connection connection = ConnectionFactory.getConnection();
             PreparedStatement stmt = connection.prepareStatement(sql);
             ResultSet rs = stmt.executeQuery()) {

            while (rs.next()) {
                Sensor sensor = new Sensor();

                sensor.setId(rs.getInt("id"));
                sensor.setNome(rs.getString("nome"));
                sensor.setVagaId(rs.getInt("vaga_id"));

                sensores.add(sensor);
            }

        } catch (SQLException e) {
            System.out.println("Erro ao listar sensores: " + e.getMessage());
        }
        return sensores;
    }

    public Sensor buscarSensorPorId(int id) {
        String sql = "SELECT * FROM sensor WHERE id = ?";

        try (Connection connection = ConnectionFactory.getConnection();
             PreparedStatement stmt = connection.prepareStatement(sql)) {

            stmt.setInt(1, id);

            try (ResultSet rs = stmt.executeQuery()) {
                if (rs.next()) {
                    Sensor sensor = new Sensor();

                    sensor.setId(rs.getInt("id"));
                    sensor.setNome(rs.getString("nome"));
                    sensor.setVagaId(rs.getInt("vaga_id"));

                    return sensor;
                }
            }
        } catch (SQLException e) {
            System.out.println("Erro ao buscar sensor por ID: " + e.getMessage());
        }
        return null;
    }

    public void atualizarSensor(Sensor sensor) {
        String sql = "UPDATE sensor SET nome = ?, vaga_id = ? WHERE id = ?";

        try (Connection connection = ConnectionFactory.getConnection();
             PreparedStatement stmt = connection.prepareStatement(sql)) {

            stmt.setString(1, sensor.getNome());
            stmt.setInt(2, sensor.getVagaId());
            stmt.setInt(3, sensor.getId());

            stmt.executeUpdate();

        } catch (SQLException e) {
            System.out.println("Erro ao atualizar sensor: " + e.getMessage());
        }
    }

    public void deletarSensor(int id) {
        String sql = "DELETE FROM sensor WHERE id = ?";

        try (Connection connection = ConnectionFactory.getConnection();
             PreparedStatement stmt = connection.prepareStatement(sql)) {

            stmt.setInt(1, id);

            stmt.executeUpdate();
        } catch (SQLException e) {
            System.out.println("Erro ao deletar sensor: " + e.getMessage());
        }
    }
}

