package com.estacionamento.api.models;

public class Vaga {
    private Integer id;
    private String numero;
    private Integer estacionamentoId;
    private Boolean estado;

    public Vaga() {
    }

    public Vaga(String numero, Integer estacionamentoId, Boolean estado) {
        this.numero = numero;
        this.estacionamentoId = estacionamentoId;
        this.estado = estado;
    }

    public Vaga(Integer id, String numero, Integer estacionamentoId, Boolean estado) {
        this.id = id;
        this.numero = numero;
        this.estacionamentoId = estacionamentoId;
        this.estado = estado;
    }

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public String getNumero() {
        return numero;
    }

    public void setNumero(String numero) {
        this.numero = numero;
    }

    public Integer getEstacionamentoId() {
        return estacionamentoId;
    }

    public void setEstacionamentoId(Integer estacionamentoId) {
        this.estacionamentoId = estacionamentoId;
    }

    public Boolean getEstado() {
        return estado;
    }

    public void setEstado(Boolean estado) {
        this.estado = estado;
    }
}