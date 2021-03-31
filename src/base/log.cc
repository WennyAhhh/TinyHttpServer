#include "log.h"

Log::Log() : thread_(std::bind(&Log::ThreadFunc, this), "Log"),
             curr_buffer_(std::make_unique<Buffer>()),
             next_buffer_(std::make_unique<Buffer>())
{
    // thread_(std::bind(&Log::threadFunc, this), "Log"),
    // auto new_thread = std::make_unique<std::thread>(std::bind(&Log::threadFunc, this), "Log");
    // thread_ = std::move(new_thread);
}

void Log::init(
    const std::string_view path,
    bool p_TimerStamp = false,
    bool p_datestamp = false)
{
    // path_ = path.data();
    time_stamp_ = p_TimerStamp;
    date_stamp_ = p_datestamp;
    SwitchLog();
    running_ = true;
}

void Log::SwitchLog()
{
    std::lock_guard<std::mutex> lk(mtx_);
    // 如果fp已经存在， 那么就需要关闭， 并且将已经存入全部去除
    curr_buffer_->RetrieveAll();
    if (next_buffer_)
    {
        next_buffer_->RetrieveAll();
    }
    buffer_.clear();
    buffer_.shrink_to_fit();
    if (fp_)
    {
        // 将buffer中的东西全存入Log
        fclose(fp_);
    }
    std::string file_name = DateDtamp() + "." + std::to_string(cnt) + ".log";
    std::string file_path = path_ + std::string("/") + file_name;
    fp_ = fopen(file_path.data(), "a");
    mkdir(path_, 0777);
    if (fp_ == nullptr)
    {
        mkdir(path_, 0777);
        fp_ = fopen(file_path.data(), "a");
    }
    is_open_ = true;
    assert(fp_ != nullptr);
}

void Log::append(const char *logline, int len)
{
    std::lock_guard<std::mutex> lk(mtx_);
    if (curr_buffer_->ReadAbleBytes() + len < SIZE)
    {
        curr_buffer_->append(logline, len);
    }
    else
    {
        buffer_.push_back(std::move(curr_buffer_));
        if (next_buffer_)
        {
            curr_buffer_ = std::move(next_buffer_);
        }
        else
        {
            curr_buffer_.reset(new Buffer);
        }
        curr_buffer_->append(logline, len);
        cond_.notify_one();
    }
}

void Log::ThreadFunc()
{
    assert(running_ == true);
    auto newBuffer1 = std::make_unique<Buffer>();
    auto newBuffer2 = std::make_unique<Buffer>();
    std::vector<std::unique_ptr<Buffer>> buffer_write;
    while (running_)
    {
        {
            std::unique_lock<std::mutex> lk(mtx_);
            if (buffer_.empty())
            {
                //cond_.wait(lk, [=] { return !buffer_.empty(); });
                cond_.wait(lk);
            }
            buffer_.push_back(std::move(curr_buffer_));
            curr_buffer_ = std::move(newBuffer1);
            buffer_write.swap(buffer_);
            if (next_buffer_ == nullptr)
            {
                next_buffer_ = std::move(newBuffer2);
            }
        }
        assert(!buffer_write.empty());
        for (auto &buffer : buffer_write)
        {
            std::string res = buffer->RetrieveAllAsString();
            fputs(res.data(), fp_);
        }
        if (buffer_write.size() > 2)
        {
            buffer_write.resize(2);
        }
        if (!newBuffer1)
        {
            assert(!buffer_write.empty());
            newBuffer1 = std::move(buffer_write.back());
            buffer_write.pop_back();
            newBuffer1->RetrieveAll();
        }
        if (!newBuffer2)
        {
            assert(!buffer_write.empty());
            newBuffer2 = std::move(buffer_write.back());
            buffer_write.pop_back();
            newBuffer2->RetrieveAll();
        }
        buffer_write.clear();
        fflush(fp_);
    }
    fflush(fp_);
}