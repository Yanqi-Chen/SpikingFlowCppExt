#include <iostream>
#include <torch/extension.h>
#include <math.h>
#include "neuron_def.h"


void LIF_hard_reset_forward_cuda(const float* x, const float* v, float* spike, float* v_next, 
    const float & v_th, const float & v_reset, const int & size, const int & gpu_id, 
    const float & reciprocal_tau);

std::vector<at::Tensor> LIF_hard_reset_forward(torch::Tensor & x, torch::Tensor & v, const float & v_th, const float & v_reset, 
    const float & reciprocal_tau)
{   
    CHECK_TENSOR(x);CHECK_TENSOR(v);
    auto spike = torch::zeros_like(v.data());
    auto v_next = torch::zeros_like(v.data());
    CHECK_TENSOR(spike);
    CHECK_TENSOR(v_next);
    LIF_hard_reset_forward_cuda(x.data_ptr<float>(), v.data_ptr<float>(), spike.data_ptr<float>(), v_next.data_ptr<float>(), 
        v_th, v_reset, x.numel(), x.get_device(),
        reciprocal_tau);
    return {spike, v_next};
}

void LIF_hard_reset_forward_with_grad_cuda(
    const float* x, const float* v, float* spike, float* v_next,
    float* grad_s_to_h, float* grad_v_to_h,
    const float & v_th, const float & v_reset, const int & size, const int & gpu_id,
    const float & alpha, const bool & detach_reset, const int & grad_surrogate_function_index,
    const float & reciprocal_tau);

std::vector<at::Tensor> LIF_hard_reset_forward_with_grad(torch::Tensor & x, torch::Tensor & v, const float & v_th, const float & v_reset,
    const float & alpha, const bool & detach_reset, const int & grad_surrogate_function_index,
    const float & reciprocal_tau)
{   
    CHECK_TENSOR(x);
    CHECK_TENSOR(v);

    auto spike = torch::zeros_like(v.data());
    auto v_next = spike.data().clone();
    auto grad_s_to_h = spike.data().clone();
    auto grad_v_to_h = spike.data().clone();

    CHECK_TENSOR(spike);
    CHECK_TENSOR(v_next);
    CHECK_TENSOR(grad_s_to_h);
    CHECK_TENSOR(grad_v_to_h);
    LIF_hard_reset_forward_with_grad_cuda(
        x.data_ptr<float>(), v.data_ptr<float>(), spike.data_ptr<float>(), v_next.data_ptr<float>(), 
        grad_s_to_h.data_ptr<float>(), grad_v_to_h.data_ptr<float>(), 
        v_th, v_reset, x.numel(), x.get_device(),
        alpha, detach_reset, grad_surrogate_function_index,
        reciprocal_tau);
    return {spike, v_next, grad_s_to_h, grad_v_to_h};
}

//fptt-------------------------------------
void LIF_hard_reset_fptt_cuda(const float* x_seq, float* spike_seq, float* v_next, 
    const float & v_th, const float & v_reset, const int & neuron_num, const int & size, const int & gpu_id, 
    const float & reciprocal_tau);

std::vector<at::Tensor> LIF_hard_reset_fptt(torch::Tensor & x_seq, torch::Tensor & v, const float & v_th, const float & v_reset, 
    const float & reciprocal_tau)
{
    CHECK_TENSOR(x_seq);
    CHECK_TENSOR(v);
    auto spike_seq = torch::zeros_like(x_seq.data());
    auto v_next = v.data().clone();
    CHECK_TENSOR(spike_seq);
    CHECK_TENSOR(v_next);
    LIF_hard_reset_fptt_cuda(x_seq.data_ptr<float>(), spike_seq.data_ptr<float>(), v_next.data_ptr<float>(), 
        v_th, v_reset, x_seq.size(1), x_seq.numel(), x_seq.get_device(),
        reciprocal_tau);
    return {spike_seq, v_next};
}

void LIF_hard_reset_fptt_with_grad_cuda(
    const float* x_seq, float* spike_seq, float* v_next, float* grad_s_to_h, float* grad_v_to_h,
    const float & v_th, const float & v_reset, 
    const int & neuron_num, const int & size, const int & gpu_id, 
    const float & alpha, const bool & detach_reset, const int & grad_surrogate_function_index,
    const float & reciprocal_tau);

std::vector<at::Tensor> LIF_hard_reset_fptt_with_grad(torch::Tensor & x_seq, torch::Tensor & v, const float & v_th, const float & v_reset, 
    const float & alpha, const bool & detach_reset, const int & grad_surrogate_function_index,
    const float & reciprocal_tau)
{
    CHECK_TENSOR(x_seq);
    CHECK_TENSOR(v);
    auto spike_seq = torch::zeros_like(x_seq.data());
    auto v_next = v.data().clone();
    auto grad_s_to_h = spike_seq.data().clone();
    auto grad_v_to_h = spike_seq.data().clone();
    CHECK_TENSOR(spike_seq);
    CHECK_TENSOR(v_next);
    CHECK_TENSOR(grad_s_to_h);
    CHECK_TENSOR(grad_v_to_h);
    LIF_hard_reset_fptt_with_grad_cuda(
        x_seq.data_ptr<float>(), spike_seq.data_ptr<float>(), v_next.data_ptr<float>(), grad_s_to_h.data_ptr<float>(), grad_v_to_h.data_ptr<float>(), 
        v_th, v_reset, 
        x_seq.size(1), x_seq.numel(), x_seq.get_device(),
        alpha, detach_reset, grad_surrogate_function_index,
        reciprocal_tau);
    return {spike_seq, v_next, grad_s_to_h, grad_v_to_h};
}

void LIF_hard_reset_backward_cuda(
  float* grad_x, float* grad_v,
  const float* grad_spike, const float* grad_v_next, const float* grad_s_to_h, const float* grad_v_to_h,
  const int & size, const int & gpu_id, 
  const float & reciprocal_tau);


std::vector<at::Tensor> LIF_hard_reset_backward(
    torch::Tensor & grad_spike, torch::Tensor & grad_v_next, torch::Tensor & grad_s_to_h, torch::Tensor & grad_v_to_h,
    const float & reciprocal_tau)
{
    CHECK_TENSOR(grad_spike);
    CHECK_TENSOR(grad_v_next);
    CHECK_TENSOR(grad_s_to_h);
    CHECK_TENSOR(grad_v_to_h);
    auto grad_x = torch::zeros_like(grad_spike.data());
    auto grad_v = grad_x.data().clone();
    CHECK_TENSOR(grad_x);
    CHECK_TENSOR(grad_v);
    LIF_hard_reset_backward_cuda(
        grad_x.data_ptr<float>(), grad_v.data_ptr<float>(),
        grad_spike.data_ptr<float>(), grad_v_next.data_ptr<float>(), grad_s_to_h.data_ptr<float>(), grad_v_to_h.data_ptr<float>(),
        grad_spike.numel(), grad_spike.get_device(),
        reciprocal_tau);
    return {grad_x, grad_v};
}


//bptt---------------------------------
void LIF_hard_reset_bptt_cuda(
  float* grad_x_seq, float* grad_v, 
  const float* grad_spike_seq, const float* grad_s_to_h, const float* grad_v_to_h,
  const int & neuron_num, const int & size, const int & gpu_id, 
  const float & reciprocal_tau);

std::vector<at::Tensor> LIF_hard_reset_bptt(
    torch::Tensor & grad_spike_seq, torch::Tensor & grad_v_next,
    torch::Tensor & grad_s_to_h, torch::Tensor & grad_v_to_h,
    const float & reciprocal_tau)
{
    CHECK_TENSOR(grad_spike_seq);
    CHECK_TENSOR(grad_v_next);
    CHECK_TENSOR(grad_s_to_h);
    CHECK_TENSOR(grad_v_to_h);
    auto grad_x_seq = torch::zeros_like(grad_spike_seq.data());
    auto grad_v = grad_v_next.data().clone();
    CHECK_TENSOR(grad_x_seq);
    CHECK_TENSOR(grad_v);

    LIF_hard_reset_bptt_cuda(
        grad_x_seq.data_ptr<float>(), grad_v.data_ptr<float>(),
        grad_spike_seq.data_ptr<float>(), grad_s_to_h.data_ptr<float>(), grad_v_to_h.data_ptr<float>(),
        grad_spike_seq.size(1), grad_spike_seq.numel(), grad_spike_seq.get_device(),
        reciprocal_tau);
    return {grad_x_seq, grad_v};
}


PYBIND11_MODULE(TORCH_EXTENSION_NAME, m) {
    m.def("LIF_hard_reset_forward", &LIF_hard_reset_forward);
    m.def("LIF_hard_reset_forward_with_grad", &LIF_hard_reset_forward_with_grad);
    m.def("LIF_hard_reset_fptt", &LIF_hard_reset_fptt);
    m.def("LIF_hard_reset_fptt_with_grad", &LIF_hard_reset_fptt_with_grad);
    m.def("LIF_hard_reset_backward", &LIF_hard_reset_backward);
    m.def("LIF_hard_reset_bptt", &LIF_hard_reset_bptt);
}


