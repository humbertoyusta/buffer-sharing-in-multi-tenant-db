$$
\sum_{t \in \text{Tenants}} \left( \frac{\text{LRU Cache hits}_t - \text{Solution Cache hits}_t}{\text{LRU Cache hits}_t} \right) ^2 \text{Priority}_t \left[\text{LRU Cache hits}_t \geq \text{Solution Cache hits}_t\right]
$$
$$
\sum_{t \in \text{Tenants}} \left( \frac{\text{Solution Cache faults}_t - \text{LRU Cache faults}_t}{\text{LRU Cache faults}_t} \right) ^2 \text{Priority}_t \left[\text{Solution Cache faults}_t \geq \text{LRU Cache faults}_t\right]
$$
