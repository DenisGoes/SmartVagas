package com.estacionamento.api.dao;

import com.estacionamento.api.database.ConnectionFactory;
import com.estacionamento.api.models.Estacionamento;

import java.sql.*;
import java.util.ArrayList;
import java.util.List;

public class EstacionamentoDAO {

    public void cadastrarEstacionamento(Estacionamento estacionamento) {
        String sql = "INSERT INTO estacionamento (nome) VALUES (?)";

        try (Connection connection = ConnectionFactory.getConnection();
             PreparedStatement stmt = connection.prepareStatement(sql)) {

            stmt.setString(1, estacionamento.getNome());

            stmt.executeUpdate();
            System.out.println("Estacionamento cadastrado com sucesso!");
        } catch (SQLException e) {
            System.out.println("Erro ao cadastrar estacionamento: " + e.getMessage());
        }
    }

    public List<Estacionamento> listarEstacionamentos() {
        List<Estacionamento> estacionamentos = new ArrayList<>();

        String sql = "SELECT * FROM estacionamento";

        try (Connection connection = ConnectionFactory.getConnection();
             PreparedStatement stmt = connection.prepareStatement(sql);
             ResultSet rs = stmt.executeQuery()) {

            while (rs.next()) {
                Estacionamento estacionamento = new Estacionamento();

                estacionamento.setId(rs.getInt("id"));
                estacionamento.setNome(rs.getString("nome"));

                estacionamentos.add(estacionamento);
            }

        } catch (SQLException e) {
            System.out.println("Erro ao listar estacionamentos: " + e.getMessage());
        }
        return estacionamentos;
    }

    public Estacionamento buscarEstacionamentoPorId(int id) {
        String sql = "SELECT * FROM estacionamento WHERE id = ?";

        try (Connection connection = ConnectionFactory.getConnection();
             PreparedStatement stmt = connection.prepareStatement(sql)) {

            stmt.setInt(1, id);

            try (ResultSet rs = stmt.executeQuery()) {
                if (rs.next()) {
                    Estacionamento estacionamento = new Estacionamento();
                    estacionamento.setId(rs.getInt("id"));
                    estacionamento.setNome(rs.getString("nome"));

                    return estacionamento;
                }
            }

        } catch (SQLException e) {
            System.out.println("Erro ao buscar estacionamento por ID: " + e.getMessage());
        }
        return null;
    }

    public void atualizarEstacionamento(Estacionamento estacionamento) {
        String sql = "UPDATE estacionamento SET nome = ? WHERE id = ?";

        try (Connection connection = ConnectionFactory.getConnection();
             PreparedStatement stmt = connection.prepareStatement(sql)) {

            stmt.setString(1, estacionamento.getNome());
            stmt.setInt(2, estacionamento.getId());

            stmt.executeUpdate();
        } catch (SQLException e) {
            System.out.println("Erro ao atualizar estacionamento: " + e.getMessage());
        }
    }

    public void deletarEstacionamento(int id) {
        String sql = "DELETE FROM estacionamento WHERE id = ?";

        try (Connection connection = ConnectionFactory.getConnection();
             PreparedStatement stmt = connection.prepareStatement(sql)) {

            stmt.setInt(1, id);

            stmt.executeUpdate();
        } catch (SQLException e) {
            System.out.println("Erro ao deletar estacionamento: " + e.getMessage());
        }
    }
}
