import numpy as np

def mape(value_model:np.array, value_obs:np.array):
    """  
    Calculate Mean Absolute Percentage Error (MAPE)
      
    Parameters:  
    value_model (np.array): Array of simulated values  
    value_obs (np.array): Array of observed values  
      
    Returns:  
    mape (float): Mean Absolute Percentage Error  
    """  

    value_obs = value_obs.astype(np.float64)
    value_model = value_model.astype(np.float64)

    all_v = np.vstack([value_model, value_obs])
    has_nan = np.isnan(all_v).any(axis=0)
    filtered_v = all_v[:, ~has_nan]

    value_model = filtered_v[0, :]
    value_obs = filtered_v[1, :]
    mape_value = np.mean(np.abs(value_obs - value_model)/value_obs) * 100
    return mape_value

def rmse(value_model:np.array, value_obs:np.array):  
    """  
    Calculate Root Mean Squared Error (RMSE)  
      
    Parameters:  
    value_obs (np.array): Array of observed values  
    value_model (np.array): Array of simulated (predicted) values  
      
    Returns:  
    rmse (float): Root Mean Squared Error  
    """  
    # 确保 obs 和 sim 的长度相同  
    assert value_obs.shape == value_model.shape, "obs and sim must have the same shape"  

    value_obs = value_obs.astype(np.float64)
    value_model = value_model.astype(np.float64)

    all_v = np.vstack([value_model, value_obs])
    has_nan = np.isnan(all_v).any(axis=0)
    filtered_v = all_v[:, ~has_nan]
    value_model = filtered_v[0, :]
    value_obs = filtered_v[1, :]
    # 计算 RMSE  
    rmse_value = np.sqrt(np.mean((value_obs - value_model) ** 2))  
    return rmse_value 

def nse(value_model:np.array, value_obs:np.array):  
    """  
    Calculate Nash-Sutcliffe Efficiency (NSE)  
      
    Parameters:  
    value_obs (np.array): Array of observed values  
    value_model (np.array): Array of simulated (predicted) values  
      
    Returns:  
    nse (float): Nash-Sutcliffe Efficiency  
    """  
    # 确保 obs 和 sim 的长度相同  
    assert value_model.shape == value_obs.shape, "obs and sim must have the same shape"  

    value_obs = value_obs.astype(np.float64)
    value_model = value_model.astype(np.float64)


    all_v = np.vstack([value_model, value_obs])
    has_nan = np.isnan(all_v).any(axis=0)
    filtered_v = all_v[:, ~has_nan]
    value_model = filtered_v[0, :]
    value_obs = filtered_v[1, :]
    # 计算观测值的平均值  
    value_obs_mean = np.mean(value_obs)  
    # 计算 NSE 的分子和分母  
    numerator = np.sum((value_obs - value_model) ** 2)  
    denominator = np.sum((value_obs - value_obs_mean) ** 2)  
    # 防止除以零（如果分母为零，通常意味着所有观测值都相同）  
    if denominator == 0:  
        return 0.0  # 或者可以返回 None, 取决于你的应用场景  
    # 计算 NSE  
    nse_value = 1 - (numerator / denominator)  
    return nse_value