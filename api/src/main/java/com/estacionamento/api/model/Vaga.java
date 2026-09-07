package com.estacionamento.api.model;

public class Vaga {
    private Integer id;
    private Integer numero;
    private String status;
    private Integer estacionamento;

    public Vaga(Integer id, Integer numero, String status, Integer estacionamento){
        this.id = id;
        this.numero = numero;
        this.status = status;
        this.estacionamento = estacionamento;
    }

    public Integer getId(){
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public Integer getNumero() {
        return numero;
    }

    public void setNumero(Integer numero) {
        this.numero = numero;
    }

    public String getStatus() {
        return status;
    }

    public void setStatus(String status) {
        this.status = status;
    }

    public Integer getEstacionamento() {
        return estacionamento;
    }

    public void setEstacionamento(Integer estacionamento) {
        this.estacionamento = estacionamento;
    }
}
