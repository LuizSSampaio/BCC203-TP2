// Declara a classe BalancedMergeFPlus1 e seus parâmetros de memória e fitas; a classe coordena a geração e intercalação dos runs, redistribui dados entre as fitas, acompanha métricas e expõe a verificação do resultado e a execução do método 2.
#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "BufferedTape.hpp"
#include "File.hpp"
#include "Item.hpp"
#include "SelectionHeap.hpp"

// Método 2: intercalação balanceada f+1 com seleção por substituição.
//
// Recursos:
//   - 19 registros em memória;
//   - 19 fitas de entrada e uma de saída.
//
// Os blocos iniciais são distribuídos entre as fitas de entrada. Em cada
// passada, os blocos correspondentes são intercalados na fita de saída e,
// se necessário, redistribuídos para a próxima passada.
class BalancedMergeFPlus1 {
  public:
    static constexpr std::size_t kMemoryRecords = 19;
    static constexpr std::size_t kInputTapes = 19;
    static constexpr std::size_t kTotalTapes = kInputTapes + 1;

    explicit BalancedMergeFPlus1(std::shared_ptr<File> file);

    // Ordena os registros e atualiza as métricas globais.
    void Sort();

    uint64_t runs() const { return runs_; }
    uint64_t mergePasses() const { return mergePasses_; }
    double phase1TimeMs() const { return phase1TimeMs_; }
    double phase2TimeMs() const { return phase2TimeMs_; }

    // Confere o resultado e, se solicitado, imprime os registros ordenados.
    // Deve ser chamado após Sort().
    bool CheckAndShowResult(bool print);

  private:
    void GenerateRuns();
    void MergePass();
    void Redistribute();
    void WriteResultToBinary();
    bool AnyInputHasRun() const;
    static void ReadOrThrow(FMais1::BufferedTape &tape, Item &item);

    std::shared_ptr<File> file_;
    uint64_t runs_ = 0;
    uint64_t mergePasses_ = 0;
    double phase1TimeMs_ = 0.0;
    double phase2TimeMs_ = 0.0;

    std::vector<std::unique_ptr<FMais1::BufferedTape>> tapes_;
    FMais1::SelectionHeap<kMemoryRecords> heap_;
    FMais1::BufferedTape *result_ = nullptr;
};

// Executa o método 2. Retorna 0 em caso de sucesso e -1 em caso de erro.
int RunMethod2(const std::shared_ptr<File> &file);
