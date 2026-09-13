package com.estacionamento.api.dao;

import com.estacionamento.api.database.ConnectionFactory;
import com.estacionamento.api.models.Vaga;

import java.sql.*;
import java.util.ArrayList;
import java.util.List;

public class VagaDAO {

    public void cadastrarVaga(Vaga vaga) {
        String sql = """
                INSERT INTO vaga
                (numero, estacionamento_id, estado)
                VALUES (?, ?, ?)
                """;
        try (Connection connection = ConnectionFactory.getConnection();
             PreparedStatement stmt = connection.prepareStatement(sql)) {

            stmt.setString(1, vaga.getNumero());
            stmt.setInt(2, vaga.getEstacionamentoId());
            stmt.setBoolean(3, vaga.getEstado());

            stmt.executeUpdate();
        } catch (SQLException e) {
            System.out.println("Erro ao criar vaga: " + e.getMessage());
        }
    }

    public List<Vaga> listarVagas() {
        List<Vaga> vagas = new ArrayList<>();

        String sql = """
                SELECT
                    id,
                    numero,
                    estacionamento_id,
                    estado
                FROM vaga
                ORDER BY id
                """;

        try (Connection connection = ConnectionFactory.getConnection();
             PreparedStatement stmt = connection.prepareStatement(sql);
             ResultSet rs = stmt.executeQuery()) {

            while (rs.next()) {
                Vaga vaga = new Vaga();

                vaga.setId(rs.getInt("id"));
                vaga.setNumero(rs.getString("numero"));
                vaga.setEstacionamentoId(rs.getInt("estacionamento_id"));
                vaga.setEstado(rs.getBoolean("estado"));

                vagas.add(vaga);
            }
        } catch (SQLException e) {
            System.out.println("Erro ao listar vagas: " + e.getMessage());
        }
        return vagas;
    }

    public Vaga buscarVagaPorId(int id) {
        String sql = """
                SELECT
                    id,
                    numero,
                    estacionamento_id,
                    estado
                FROM vaga
                WHERE id = ?
                """;

        try (Connection connection = ConnectionFactory.getConnection();
             PreparedStatement stmt = connection.prepareStatement(sql)) {

            stmt.setInt(1, id);

            try (ResultSet rs = stmt.executeQuery()) {
                if (rs.next()) {
                    Vaga vaga = new Vaga();

                    vaga.setId(rs.getInt("id"));
                    vaga.setNumero(rs.getString("numero"));
                    vaga.setEstacionamentoId(rs.getInt("estacionamento_id"));
                    vaga.setEstado(rs.getBoolean("estado"));

                    return vaga;
                }
            }
        } catch (SQLException e) {
            System.out.println("Erro ao buscar vaga por ID: " + e.getMessage());
        }
        return null;
    }

    public void atualizarVaga(Vaga vaga) {
        String sql = """
                UPDATE vaga
                SET numero = ?,
                    estacionamento_id = ?
                WHERE id = ?
                """;

        try (Connection connection = ConnectionFactory.getConnection(); PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setString(1, vaga.getNumero());
            stmt.setInt(2, vaga.getEstacionamentoId());
            stmt.setInt(3, vaga.getId());

            stmt.executeUpdate();
        } catch (SQLException e) {
            System.out.println("Erro ao atualizar vaga: " + e.getMessage());
        }
    }

    public void deletarVaga(int id) {
        String sql = "DELETE FROM vaga WHERE id = ?";

        try (Connection connection = ConnectionFactory.getConnection(); PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setInt(1, id);

            stmt.executeUpdate();
        } catch (SQLException e) {
            System.out.println("Erro ao deletar vaga: " + e.getMessage());
        }
    }
}