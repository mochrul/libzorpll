/***************************************************************************
 *
 * This file is covered by a dual licence. You can choose whether you
 * want to use it according to the terms of the GNU GPL version 2, or
 * under the terms of Zorp Professional Firewall System EULA located
 * on the Zorp installation CD.
 *
 ***************************************************************************/
#include <glib.h>

#include <glib.h>
#ifdef G_OS_WIN32

#include <zorp/win32_reg.h>
#include <string.h>

#include <windows.h>
#include <winreg.h>

/**
 * This function stores data in the value field of an registry key.
 *
 * @param[in] root The key that is opened or created by this function is a subkey of the key that is identified by this parameter.
 * @param[in] key Pointer to a null-terminated string that specifies the name of a subkey that this function opens or creates.
 * This is a subkey of the key that is identified by <TT>root</TT>.
 * @param[in] name Pointer to a string containing the name of the value to set.
 * @param[in] value The data to be stored with the specified value name.
*/
gboolean
z_reg_key_write_dword(HKEY root, gchar *key, gchar *name, DWORD value)
{
  HKEY hKey;

  if(RegCreateKeyEx(root, (LPCSTR)key, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL) != ERROR_SUCCESS)
    {
      return FALSE;
    }

  if(RegSetValueEx(hKey, (LPCSTR)name, 0, REG_DWORD, (LPBYTE)&value, sizeof(DWORD)) == ERROR_SUCCESS)
    {
      RegFlushKey(hKey);
      RegCloseKey(hKey);
      return TRUE;
    }

  RegCloseKey(hKey);
  return FALSE;
}

/**
 * This function stores data in the value field of an registry key.
 *
 * @param[in] root The key that is opened or created by this function is a subkey of the key that is identified by this parameter.
 * @param[in] key Pointer to a null-terminated string that specifies the name of a subkey that this function opens or creates.
 * This is a subkey of the key that is identified by <TT>root</TT>.
 * @param[in] name Pointer to a string containing the name of the value to set.
 * @param[in] value Pointer to a buffer that contains the data to be stored with the specified value name.
*/
gboolean
z_reg_key_write_string(HKEY root, gchar *key, gchar *name, gchar *value)
{
  HKEY hKey;

  if(RegCreateKeyEx(root, (LPCSTR)key, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL) != ERROR_SUCCESS)
    return FALSE;

  if(RegSetValueEx(hKey, (LPCSTR)name, 0, REG_SZ, (LPBYTE)value, strlen(value)+1) == ERROR_SUCCESS)
    {
      RegFlushKey(hKey);
      RegCloseKey(hKey);
      return TRUE;
    }

  RegCloseKey(hKey);
  return FALSE;
}

/**
 * This function stores data in the value field of an registry key.
 *
 * @param[in] root The key that is removed by this function is a subkey of the key that is identified by this parameter.
 * @param[in] key Pointer to a null-terminated string that specifies the name of a subkey that this function removes.
 * This is a subkey of the key that is identified by <TT>root</TT>.
 * @param[in] name Pointer to a null-terminated string that names the value to remove.
*/
gboolean
z_reg_key_delete(HKEY root, gchar *key, gchar *name)
{
  HKEY hKey;

  if(RegOpenKeyEx(root,(LPCSTR)key, 0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS)
    return FALSE;

  if(RegDeleteValue(hKey, (LPCSTR)name) != ERROR_SUCCESS)
    {
      RegFlushKey(hKey);
      RegCloseKey(hKey);
      return TRUE;
    }

  RegCloseKey(hKey);
  return FALSE;
}

/**
 * This function retrieves the data for a specified value associated with a specified registry key.
 *
 * @param[in] root The key that is opened or created by this function is a subkey of the key that is identified by this parameter.
 * @param[in] key Pointer to a null-terminated string that specifies the name of a subkey that this function opens or creates.
 * This is a subkey of the key that is identified by <TT>root</TT>.
 * @param[in] name Pointer to a string containing the name of the value to set.
 * @param[in, out] value Pointer to a buffer that receives value data.
*/
gboolean
z_reg_key_read_dword(HKEY root, gchar *key, gchar *name, DWORD *value)
{
  HKEY hKey;
  DWORD t = sizeof(DWORD);
  DWORD type = REG_DWORD;

  if(RegOpenKeyEx(root,(LPCSTR)key, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
    return FALSE;

  if(RegQueryValueEx(hKey, (LPCSTR)name, 0, &type, (LPBYTE)value, &t) == ERROR_SUCCESS)
    {
      RegCloseKey(hKey);
      return TRUE;
    }
  RegCloseKey(hKey);
  return FALSE;
}

/**
 * This function retrieves the data for a specified value associated with a specified registry key.
 *
 * @param[in] root The key that is opened or created by this function is a subkey of the key that is identified by this parameter.
 * @param[in] key Pointer to a null-terminated string that specifies the name of a subkey that this function opens or creates.
 * This is a subkey of the key that is identified by <TT>root</TT>.
 * @param[in] name Pointer to a string containing the name of the value to set.
 * @param[in, out] value Pointer to a buffer that receives value data.
*/
gboolean
z_reg_key_read_string(HKEY root, gchar *key, gchar *name, gchar **value)
{
  HKEY hKey;
  gchar temp[2000];
  DWORD t = 2000;
  DWORD type = REG_SZ;

  if(RegOpenKeyEx(root,(LPCSTR)key, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
    return FALSE;

  if(RegQueryValueEx(hKey, (LPCSTR)name, 0, &type, (LPBYTE)&temp, &t) == ERROR_SUCCESS)
    {
      *value = g_strdup(temp);
      RegCloseKey(hKey);
      return TRUE;
    }

  RegCloseKey(hKey);
  return FALSE;
}

gboolean
z_sid_to_text( PSID ps, char *buf, int bufSize )
{
  ZRegistryForeachCallbackData helper_data;

  g_assert(z_registry_is_type_valid(type));

  helper_data.user_func = func;
  helper_data.user_data = user_data;

  g_hash_table_foreach(registry[type], z_registry_foreach_invoke_callback, &helper_data);
}
