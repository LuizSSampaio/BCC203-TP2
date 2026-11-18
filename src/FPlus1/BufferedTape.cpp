// Implementa a fita com buffer de página: acumula registros em blocos para escrita, lê os blocos sequencialmente e mantém o tamanho e a posição dos runs armazenados, além de encaminhar as métricas de I/O para a fita subjacente.
#include "BufferedTape.hpp"

namespace FMais1 {

BufferedTape::BufferedTape() : tape_(std::make_unique<Tape>()) {}

void BufferedTape::Clear() {
    this->tape_->Clear();
    this->runs_.clear();
    this->bufPos_ = 0;
    this->currentRunLength_ = 0;
    this->totalRecords_ = 0;
    this->recordsRead_ = 0;
    this->nextRun_ = 0;
    this->reading_ = false;
}

void BufferedTape::WriteRecord(const Item &item) {
    this->buffer_[this->bufPos_++] = item;
    ++this->currentRunLength_;
    ++this->totalRecords_;

    if (this->bufPos_ == PAGE_SIZE) {
        this->tape_->WriteBlock(this->buffer_);
        this->bufPos_ = 0;
    }
}

void BufferedTape::EndRun() {
    if (this->currentRunLength_ > 0) {
        this->runs_.push_back(this->currentRunLength_);
        this->currentRunLength_ = 0;
    }
}

void BufferedTape::Flush() {
    this->EndRun();

    if (!this->reading_ && this->bufPos_ > 0) {
        this->tape_->WriteBlock(this->buffer_);
        this->bufPos_ = 0;
    }
}

void BufferedTape::Rewind() {
    this->Flush();

    this->tape_->Rewind();
    this->bufPos_ = PAGE_SIZE;
    this->recordsRead_ = 0;
    this->nextRun_ = 0;
    this->reading_ = true;
}

bool BufferedTape::ReadRecord(Item &item) {
    if (this->recordsRead_ >= this->totalRecords_) {
        return false;
    }

    if (this->bufPos_ >= PAGE_SIZE) {
        this->buffer_ = this->tape_->ReadBlock();
        this->bufPos_ = 0;
    }

    item = this->buffer_[this->bufPos_++];
    ++this->recordsRead_;
    return true;
}

bool BufferedTape::HasNextRun() const {
    return this->nextRun_ < this->runs_.size();
}

uint64_t BufferedTape::BeginNextRun() {
    return this->runs_[this->nextRun_++];
}

std::size_t BufferedTape::NumRuns() const { return this->runs_.size(); }

void BufferedTape::SetRecordMetrics(bool enabled) {
    this->tape_->SetRecordMetrics(enabled);
}

}  // namespace FMais1
