// =====================================================
// CONFIGURAÇÃO
// =====================================================

const API = "/smartvagas";

// =====================================================
// DADOS CARREGADOS DA API
// =====================================================

let estacionamentos = [];
let vagas = [];
let sensores = [];
let leituras = [];

// =====================================================
// MENU
// =====================================================

function mostrarPagina(id, botao) {

    document.querySelectorAll(".pagina")
        .forEach(pagina => {
            pagina.classList.remove("ativa");
        });

    const pagina = document.getElementById(id);

    if (pagina) {
        pagina.classList.add("ativa");
    }

    document.querySelectorAll(".menu")
        .forEach(menu => {
            menu.classList.remove("ativo");
        });

    if (botao) {
        botao.classList.add("ativo");
    }

    if (id === "leituras") {
        carregarLeituras();
    }
}

// =====================================================
// REQUISIÇÃO GENÉRICA
// =====================================================

async function requisicao(url, opcoes = {}) {

    try {

        const resposta = await fetch(url, {
            ...opcoes,
            cache: "no-store",

            headers: {
                "Content-Type": "application/json",
                ...(opcoes.headers || {})
            }
        });

        const texto = await resposta.text();

        // =============================================
        // ERRO HTTP
        // =============================================

        if (!resposta.ok) {

            throw new Error(
                texto || `Erro HTTP ${resposta.status}`
            );
        }

        // =============================================
        // RESPOSTA VAZIA
        // =============================================

        if (!texto.trim()) {
            return null;
        }

        // =============================================
        // TENTA JSON
        // =============================================

        try {

            return JSON.parse(texto);

        } catch {

            // =========================================
            // API RETORNOU TEXTO PURO
            // =========================================

            return texto;
        }

    } catch (erro) {

        console.error("Erro na requisição:", erro);

        throw erro;
    }
}

// =====================================================
// CARREGAR TUDO
// =====================================================

async function carregarSistema() {

    try {

        await Promise.all([
            carregarEstacionamentos(),
            carregarVagas(),
            carregarSensores()
        ]);

        atualizarDashboard();
        atualizarSelects();

        renderizarVagas();
        renderizarEstacionamentos();
        renderizarListaVagas();
        renderizarSensores();

    } catch (erro) {

        console.error(
            "Erro ao carregar sistema:",
            erro
        );
    }
}

// =====================================================
// ESTACIONAMENTOS - GET
// =====================================================

async function carregarEstacionamentos() {

    const resultado = await requisicao(
        `${API}/estacionamentos`
    );

    estacionamentos = Array.isArray(resultado)
        ? resultado
        : [];
}

// =====================================================
// ESTACIONAMENTOS - POST
// =====================================================

async function adicionarEstacionamento() {

    const campo =
        document.getElementById("nomeEstacionamento");

    const nome = campo.value.trim();

    if (!nome) {

        alert(
            "Digite o nome do estacionamento."
        );

        return;
    }

    try {

        await requisicao(
            `${API}/estacionamentos`,
            {
                method: "POST",

                body: JSON.stringify({
                    nome: nome
                })
            }
        );

        campo.value = "";

        await carregarSistema();

        alert(
            "Estacionamento cadastrado com sucesso!"
        );

    } catch (erro) {

        console.error(erro);

        alert(
            "Erro ao cadastrar estacionamento.\n\n"
            + erro.message
        );
    }
}

// =====================================================
// ESTACIONAMENTOS - PUT
// =====================================================

async function editarEstacionamento(id) {

    const estacionamento =
        estacionamentos.find(
            item => Number(item.id) === Number(id)
        );

    if (!estacionamento) {
        return;
    }

    const novoNome =
        prompt(
            "Novo nome:",
            estacionamento.nome
        );

    if (!novoNome || !novoNome.trim()) {
        return;
    }

    try {

        await requisicao(
            `${API}/estacionamentos/${id}`,
            {
                method: "PUT",

                body: JSON.stringify({
                    nome: novoNome.trim()
                })
            }
        );

        await carregarSistema();

    } catch (erro) {

        console.error(erro);

        alert(
            "Erro ao atualizar estacionamento.\n\n"
            + erro.message
        );
    }
}

// =====================================================
// ESTACIONAMENTOS - DELETE
// =====================================================

async function deletarEstacionamento(id) {

    if (
        !confirm(
            "Tem certeza que deseja excluir este estacionamento?"
        )
    ) {
        return;
    }

    try {

        await requisicao(
            `${API}/estacionamentos/${id}`,
            {
                method: "DELETE"
            }
        );

        // Recarrega os dados
        await carregarSistema();

        // Verifica se realmente desapareceu
        const aindaExiste =
            estacionamentos.some(
                estacionamento =>
                    Number(estacionamento.id) === Number(id)
            );

        if (aindaExiste) {

            alert(
                "O estacionamento não foi excluído.\n\n"
                + "Provavelmente existem vagas vinculadas "
                + "a este estacionamento."
            );

            return;
        }

        alert(
            "Estacionamento excluído com sucesso!"
        );

    } catch (erro) {

        console.error(erro);

        alert(
            "Não foi possível excluir o estacionamento.\n\n"
            + erro.message
        );
    }
}

// =====================================================
// VAGAS - GET
// =====================================================

async function carregarVagas() {

    const resultado = await requisicao(
        `${API}/vagas`
    );

    vagas = Array.isArray(resultado)
        ? resultado.map(vaga => ({
            ...vaga,
            estado:
                vaga.estado === true
                || vaga.estado === 1
                || vaga.estado === "true"
                || vaga.estado === "1"
        }))
        : [];
}

// =====================================================
// VAGAS - POST
// =====================================================

async function adicionarVaga() {

    const estacionamentoId =
        Number(
            document
                .getElementById("estacionamentoVaga")
                .value
        );

    const numero =
        document
            .getElementById("numeroVaga")
            .value
            .trim()
            .toUpperCase();

    if (!estacionamentoId) {

        alert(
            "Selecione um estacionamento."
        );

        return;
    }

    if (!numero) {

        alert(
            "Digite o número da vaga."
        );

        return;
    }

    try {

        await requisicao(
            `${API}/vagas`,
            {
                method: "POST",

                body: JSON.stringify({
                    numero: numero,
                    estacionamentoId: estacionamentoId,
                    estado: false
                })
            }
        );

        document
            .getElementById("numeroVaga")
            .value = "";

        await carregarSistema();

        alert(
            "Vaga cadastrada com sucesso!"
        );

    } catch (erro) {

        console.error(erro);

        alert(
            "Erro ao cadastrar vaga.\n\n"
            + erro.message
        );
    }
}

// =====================================================
// VAGAS - PUT
// =====================================================

async function editarVaga(id) {

    const vaga =
        vagas.find(
            item => Number(item.id) === Number(id)
        );

    if (!vaga) {
        return;
    }

    const novoNumero =
        prompt(
            "Número da vaga:",
            vaga.numero
        );

    if (!novoNumero || !novoNumero.trim()) {
        return;
    }

    try {

        await requisicao(
            `${API}/vagas/${id}`,
            {
                method: "PUT",

                body: JSON.stringify({
                    numero:
                        novoNumero
                            .trim()
                            .toUpperCase(),

                    estacionamentoId:
                        vaga.estacionamentoId,

                    estado:
                        vaga.estado
                })
            }
        );

        await carregarSistema();

    } catch (erro) {

        console.error(erro);

        alert(
            "Erro ao atualizar vaga.\n\n"
            + erro.message
        );
    }
}

// =====================================================
// VAGAS - DELETE
// =====================================================

async function deletarVaga(id) {

    if (
        !confirm(
            "Deseja excluir esta vaga?"
        )
    ) {
        return;
    }

    try {

        await requisicao(
            `${API}/vagas/${id}`,
            {
                method: "DELETE"
            }
        );

        // Recarrega do banco
        await carregarSistema();

        // Verifica se realmente foi excluída
        const aindaExiste =
            vagas.some(
                vaga =>
                    Number(vaga.id) === Number(id)
            );

        if (aindaExiste) {

            const possuiSensor =
                sensores.some(
                    sensor =>
                        Number(sensor.vagaId)
                        === Number(id)
                );

            if (possuiSensor) {

                alert(
                    "A vaga não pode ser excluída.\n\n"
                    + "Existe um sensor vinculado a esta vaga.\n\n"
                    + "Exclua primeiro o sensor."
                );

            } else {

                alert(
                    "A vaga não foi excluída.\n\n"
                    + "O banco de dados recusou a operação."
                );
            }

            return;
        }

        alert(
            "Vaga excluída com sucesso!"
        );

    } catch (erro) {

        console.error(erro);

        alert(
            "Erro ao excluir vaga.\n\n"
            + erro.message
        );
    }
}

// =====================================================
// SENSORES - GET
// =====================================================

async function carregarSensores() {

    const resultado = await requisicao(
        `${API}/sensores`
    );

    sensores = Array.isArray(resultado)
        ? resultado
        : [];
}

// =====================================================
// SENSORES - POST
// =====================================================

async function adicionarSensor() {

    const nome =
        document
            .getElementById("nomeSensor")
            .value
            .trim();

    const vagaId =
        Number(
            document
                .getElementById("vagaSensor")
                .value
        );

    if (!nome) {

        alert(
            "Digite o nome do sensor."
        );

        return;
    }

    if (!vagaId) {

        alert(
            "Selecione uma vaga."
        );

        return;
    }

    try {

        await requisicao(
            `${API}/sensores`,
            {
                method: "POST",

                body: JSON.stringify({
                    nome: nome,
                    vagaId: vagaId
                })
            }
        );

        document
            .getElementById("nomeSensor")
            .value = "";

        await carregarSistema();

        alert(
            "Sensor cadastrado com sucesso!"
        );

    } catch (erro) {

        console.error(erro);

        alert(
            "Erro ao cadastrar sensor.\n\n"
            + erro.message
        );
    }
}

// =====================================================
// SENSOR - PUT
// =====================================================

async function editarSensor(id) {

    const sensor =
        sensores.find(
            item => Number(item.id) === Number(id)
        );

    if (!sensor) {
        return;
    }

    const novoNome =
        prompt(
            "Nome do sensor:",
            sensor.nome
        );

    if (!novoNome || !novoNome.trim()) {
        return;
    }

    try {

        await requisicao(
            `${API}/sensores/${id}`,
            {
                method: "PUT",

                body: JSON.stringify({
                    nome:
                        novoNome.trim(),

                    vagaId:
                        sensor.vagaId
                })
            }
        );

        await carregarSistema();

    } catch (erro) {

        console.error(erro);

        alert(
            "Erro ao atualizar sensor.\n\n"
            + erro.message
        );
    }
}

// =====================================================
// SENSOR - DELETE
// =====================================================

async function deletarSensor(id) {

    if (
        !confirm(
            "Deseja excluir este sensor?"
        )
    ) {
        return;
    }

    try {

        await requisicao(
            `${API}/sensores/${id}`,
            {
                method: "DELETE"
            }
        );

        // Recarrega do banco
        await carregarSistema();

        // Verifica se realmente desapareceu
        const aindaExiste =
            sensores.some(
                sensor =>
                    Number(sensor.id) === Number(id)
            );

        if (aindaExiste) {

            const possuiLeituras =
                leituras.some(
                    leitura =>
                        Number(leitura.sensorId)
                        === Number(id)
                );

            if (possuiLeituras) {

                alert(
                    "O sensor não pode ser excluído.\n\n"
                    + "Existem leituras vinculadas a este sensor.\n\n"
                    + "Exclua primeiro as leituras deste sensor."
                );

            } else {

                alert(
                    "O sensor não foi excluído.\n\n"
                    + "O banco de dados recusou a operação."
                );
            }

            return;
        }

        alert(
            "Sensor excluído com sucesso!"
        );

    } catch (erro) {

        console.error(erro);

        alert(
            "Erro ao excluir sensor.\n\n"
            + erro.message
        );
    }
}

// =====================================================
// DASHBOARD
// =====================================================

function atualizarDashboard() {

    const total =
        vagas.length;

    const ocupadas =
        vagas.filter(
            vaga => vaga.estado === true
        ).length;

    const livres =
        total - ocupadas;

    const totalVagas =
        document.getElementById("totalVagas");

    const vagasLivres =
        document.getElementById("vagasLivres");

    const vagasOcupadas =
        document.getElementById("vagasOcupadas");

    const totalEstacionamentos =
        document.getElementById(
            "totalEstacionamentos"
        );

    if (totalVagas) {
        totalVagas.textContent = total;
    }

    if (vagasLivres) {
        vagasLivres.textContent = livres;
    }

    if (vagasOcupadas) {
        vagasOcupadas.textContent = ocupadas;
    }

    if (totalEstacionamentos) {
        totalEstacionamentos.textContent =
            estacionamentos.length;
    }
}

// =====================================================
// SELECTS
// =====================================================

function atualizarSelects() {

    const filtro =
        document.getElementById(
            "filtroEstacionamento"
        );

    const estacionamentoVaga =
        document.getElementById(
            "estacionamentoVaga"
        );

    const vagaSensor =
        document.getElementById(
            "vagaSensor"
        );

    // =============================================
    // GUARDA SELEÇÃO ATUAL
    // =============================================

    const filtroAtual =
        filtro ? filtro.value : "";

    const estacionamentoAtual =
        estacionamentoVaga
            ? estacionamentoVaga.value
            : "";

    const vagaAtual =
        vagaSensor
            ? vagaSensor.value
            : "";

    // =============================================
    // LIMPA
    // =============================================

    if (filtro) {
        filtro.innerHTML = "";
    }

    if (estacionamentoVaga) {
        estacionamentoVaga.innerHTML = "";
    }

    if (vagaSensor) {
        vagaSensor.innerHTML = "";
    }

    // =============================================
    // ESTACIONAMENTOS
    // =============================================

    estacionamentos.forEach(
        estacionamento => {

            if (filtro) {

                filtro.innerHTML += `
                    <option value="${estacionamento.id}">
                        ${estacionamento.nome}
                    </option>
                `;
            }

            if (estacionamentoVaga) {

                estacionamentoVaga.innerHTML += `
                    <option value="${estacionamento.id}">
                        ${estacionamento.nome}
                    </option>
                `;
            }
        }
    );

    // =============================================
    // VAGAS
    // =============================================

    vagas.forEach(vaga => {

        if (vagaSensor) {

            vagaSensor.innerHTML += `
                <option value="${vaga.id}">
                    ${vaga.numero}
                </option>
            `;
        }
    });

    // =============================================
    // RESTAURA SELEÇÃO
    // =============================================

    if (
        filtro &&
        [...filtro.options].some(
            option => option.value === filtroAtual
        )
    ) {
        filtro.value = filtroAtual;
    }

    if (
        estacionamentoVaga &&
        [...estacionamentoVaga.options].some(
            option =>
                option.value === estacionamentoAtual
        )
    ) {
        estacionamentoVaga.value =
            estacionamentoAtual;
    }

    if (
        vagaSensor &&
        [...vagaSensor.options].some(
            option => option.value === vagaAtual
        )
    ) {
        vagaSensor.value = vagaAtual;
    }
}

// =====================================================
// MAPA DE VAGAS
// =====================================================

function renderizarVagas() {

    const mapa =
        document.getElementById(
            "mapaVagas"
        );

    if (!mapa) {
        return;
    }

    mapa.innerHTML = "";

    const select =
        document.getElementById(
            "filtroEstacionamento"
        );

    if (!select || !select.value) {

        mapa.innerHTML =
            "<p>Nenhum estacionamento cadastrado.</p>";

        return;
    }

    const estacionamentoId =
        Number(select.value);

    const vagasFiltradas =
        vagas.filter(
            vaga =>
                Number(vaga.estacionamentoId)
                === estacionamentoId
        );

    if (
        vagasFiltradas.length === 0
    ) {

        mapa.innerHTML = `
            <p class="sem-vagas">
                Nenhuma vaga cadastrada neste estacionamento.
            </p>
        `;

        return;
    }

    vagasFiltradas.forEach(vaga => {

        const ocupada =
            vaga.estado === true;

        const classe =
            ocupada
                ? "vaga-ocupada"
                : "vaga-livre";

        mapa.innerHTML += `
            <div class="vaga ${classe}">

                <h3>
                    ${vaga.numero}
                </h3>

                <span>
                    ${ocupada ? "🚘" : "✓"}
                </span>

                <small>
                    ${ocupada ? "Ocupada" : "Livre"}
                </small>

            </div>
        `;
    });
}

// =====================================================
// LISTA DE ESTACIONAMENTOS
// =====================================================

function renderizarEstacionamentos() {

    const lista =
        document.getElementById(
            "listaEstacionamentos"
        );

    if (!lista) {
        return;
    }

    lista.innerHTML = "";

    estacionamentos.forEach(
        estacionamento => {

            const quantidade =
                vagas.filter(
                    vaga =>
                        Number(
                            vaga.estacionamentoId
                        )
                        ===
                        Number(
                            estacionamento.id
                        )
                ).length;

            lista.innerHTML += `
                <div class="item-lista">

                    <h3>
                        🏢 ${estacionamento.nome}
                    </h3>

                    <p>
                        🚗 ${quantidade} vagas
                    </p>

                    <div class="acoes">

                        <button
                            onclick="editarEstacionamento(${estacionamento.id})"
                            class="editar">
                            ✏️ Editar
                        </button>

                        <button
                            onclick="deletarEstacionamento(${estacionamento.id})"
                            class="excluir">
                            🗑️ Excluir
                        </button>

                    </div>

                </div>
            `;
        }
    );
}

// =====================================================
// LISTA DE VAGAS
// =====================================================

function renderizarListaVagas() {

    const lista =
        document.getElementById(
            "listaVagas"
        );

    if (!lista) {
        return;
    }

    lista.innerHTML = "";

    vagas.forEach(vaga => {

        const estacionamento =
            estacionamentos.find(
                item =>
                    Number(item.id)
                    ===
                    Number(vaga.estacionamentoId)
            );

        const ocupada =
            vaga.estado === true;

        lista.innerHTML += `
            <div class="item-lista">

                <h3>
                    🚘 Vaga ${vaga.numero}
                </h3>

                <p>
                    🏢
                    ${
                        estacionamento
                            ? estacionamento.nome
                            : "Desconhecido"
                    }
                </p>

                <p>
                    Status:

                    <strong class="${
                        ocupada
                            ? "texto-ocupada"
                            : "texto-livre"
                    }">

                        ${
                            ocupada
                                ? "🔴 Ocupada"
                                : "🟢 Livre"
                        }

                    </strong>
                </p>

                <div class="acoes">

                    <button
                        onclick="editarVaga(${vaga.id})"
                        class="editar">
                        ✏️ Editar
                    </button>

                    <button
                        onclick="deletarVaga(${vaga.id})"
                        class="excluir">
                        🗑️ Excluir
                    </button>

                </div>

            </div>
        `;
    });
}

// =====================================================
// LISTA DE SENSORES
// =====================================================

function renderizarSensores() {

    const lista =
        document.getElementById(
            "listaSensores"
        );

    if (!lista) {
        return;
    }

    lista.innerHTML = "";

    sensores.forEach(sensor => {

        const vaga =
            vagas.find(
                item =>
                    Number(item.id)
                    ===
                    Number(sensor.vagaId)
            );

        lista.innerHTML += `
            <div class="item-lista">

                <h3>
                    📡 ${sensor.nome}
                </h3>

                <p>
                    🚘 Vaga:

                    ${
                        vaga
                            ? vaga.numero
                            : "Não encontrada"
                    }
                </p>

                <div class="acoes">

                    <button
                        onclick="editarSensor(${sensor.id})"
                        class="editar">
                        ✏️ Editar
                    </button>

                    <button
                        onclick="deletarSensor(${sensor.id})"
                        class="excluir">
                        🗑️ Excluir
                    </button>

                </div>

            </div>
        `;
    });
}

// =====================================================
// LEITURAS - GET
// =====================================================

async function carregarLeituras() {

    try {

        const resultado =
            await requisicao(
                `${API}/leituras`
            );

        leituras =
            Array.isArray(resultado)
                ? resultado
                : [];

        renderizarLeituras();

    } catch (erro) {

        console.error(erro);

        alert(
            "Erro ao carregar leituras.\n\n"
            + erro.message
        );
    }
}

// =====================================================
// LEITURAS - RENDERIZAÇÃO
// =====================================================

function renderizarLeituras() {

    const tabela =
        document.getElementById(
            "tabelaLeituras"
        );

    if (!tabela) {
        return;
    }

    tabela.innerHTML = "";

    leituras.forEach(leitura => {

        tabela.innerHTML += `
            <tr>

                <td>
                    ${leitura.id ?? "-"}
                </td>

                <td>
                    ${leitura.sensorId ?? "-"}
                </td>

                <td>

                    <strong class="${
                        leitura.estado
                            ? "texto-ocupada"
                            : "texto-livre"
                    }">

                        ${
                            leitura.estado
                                ? "🔴 Ocupada"
                                : "🟢 Livre"
                        }

                    </strong>

                </td>

                <td>
                    ${formatarData(leitura.dataHora)}
                </td>

                <td>

                    <button
                        class="excluir"
                        onclick="deletarLeitura(${leitura.id})">

                        🗑️

                    </button>

                </td>

            </tr>
        `;
    });
}

// =====================================================
// LEITURA - DELETE
// =====================================================

async function deletarLeitura(id) {

    if (
        !confirm(
            "Deseja realmente deletar esta leitura?"
        )
    ) {
        return;
    }

    try {

        await requisicao(
            `${API}/leituras/${id}`,
            {
                method: "DELETE"
            }
        );

        await carregarLeituras();

        // Verifica se realmente saiu
        const aindaExiste =
            leituras.some(
                leitura =>
                    Number(leitura.id)
                    === Number(id)
            );

        if (aindaExiste) {

            alert(
                "A leitura não foi excluída."
            );

            return;
        }

        console.log(
            "Leitura deletada com sucesso!"
        );

    } catch (erro) {

        console.error(erro);

        alert(
            "Não foi possível deletar a leitura.\n\n"
            + erro.message
        );
    }
}
async function deletarTodasLeituras() {

    if (!confirm(
        "ATENÇÃO!\n\n" +
        "Isso irá excluir TODAS as leituras dos sensores.\n\n" +
        "Essa operação não pode ser desfeita.\n\n" +
        "Deseja continuar?"
    )) {
        return;
    }

    try {

        await requisicao(`${API}/leituras`, {
            method: "DELETE"
        });

        await carregarLeituras();

        alert("Todas as leituras foram excluídas com sucesso!");

    } catch (erro) {

        console.error("Erro ao excluir todas as leituras:", erro);

        alert(
            "Erro ao excluir todas as leituras.\n\n" +
            erro.message
        );
    }
}

// =====================================================
// DATA
// =====================================================

function formatarData(data) {

    if (!data) {
        return "-";
    }

    try {

        return new Date(data)
            .toLocaleString(
                "pt-BR"
            );

    } catch {

        return data;
    }
}

// =====================================================
// ATUALIZAÇÃO EM TEMPO REAL
// =====================================================
//
// ESP32
//   ↓
// API
//   ↓
// Banco
//   ↓
// Frontend consulta /vagas
//   ↓
// Dashboard atualiza
//
// =====================================================

async function atualizarTempoReal() {

    try {

        await carregarVagas();

        atualizarDashboard();
        atualizarSelects();

        renderizarVagas();
        renderizarListaVagas();

    } catch (erro) {

        console.error(
            "Erro na atualização:",
            erro
        );
    }
}

// =====================================================
// INICIALIZAÇÃO
// =====================================================

carregarSistema();

// =====================================================
// ATUALIZA VAGAS A CADA 1 SEGUNDO
// =====================================================

setInterval(
    atualizarTempoReal,
    1000
);
