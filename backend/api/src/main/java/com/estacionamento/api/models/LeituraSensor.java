package com.estacionamento.api.models;

import java.time.OffsetDateTime;

public class LeituraSensor {
    private Integer id;
    private Integer sensorId;
    private Boolean estado;
    private OffsetDateTime dataHora;

    public LeituraSensor() {
    }

    public LeituraSensor(Integer sensorId, Boolean estado) {
        this.sensorId = sensorId;
        this.estado = estado;
    }

    public LeituraSensor(Integer sensorId, Boolean estado, OffsetDateTime dataHora) {
        this.sensorId = sensorId;
        this.estado = estado;
        this.dataHora = dataHora;
    }

    public LeituraSensor(Integer id, Integer sensorId, Boolean estado, OffsetDateTime dataHora) {
        this.id = id;
        this.sensorId = sensorId;
        this.estado = estado;
        this.dataHora = dataHora;
    }

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public Integer getSensorId() {
        return sensorId;
    }

    public void setSensorId(Integer sensorId) {
        this.sensorId = sensorId;
    }

    public Boolean getEstado() {
        return estado;
    }

    public void setEstado(Boolean estado) {
        this.estado = estado;
    }

    public OffsetDateTime getDataHora() {
        return dataHora;
    }

    public void setDataHora(OffsetDateTime dataHora) {
        this.dataHora = dataHora;
    }
}