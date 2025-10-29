#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INITIAL_PRODUCTS 100
#define NAME_LENGTH 50
#define MAX_ID 10000
#define MAX_PRICE 100000
#define MAX_QUANTITY 1000000

typedef struct
{
  int id;
  char name[NAME_LENGTH];
  float price;
  int quantity;
} Product;

int findProductIndexByID(const Product *productList, int id, int size)
{
  for (int index = 0; index < size; index++)
  {
    if (productList[index].id == id)
    {
      return index;
    }
  }
  return -1;
}

void printProduct(const Product object)
{
  printf("Product ID: %d | ", object.id);
  printf("Name: %s | ", object.name);
  printf("Price: %.2f | ", object.price);
  printf("Quantity: %d\n", object.quantity);
}

int stringCompare(const char *target, const char *toSearch)
{
  while (*toSearch != '\0')
  {
    if (tolower(*target) != tolower(*toSearch))
    {
      return 0;
    }
    target++;
    toSearch++;
  }
  return 1;
}

int containsIgnoreCase(const char *targetString, const char *searchTerm)
{
  while (*targetString != '\0')
  {
    if (tolower(*targetString) == tolower(searchTerm[0]))
    {
      if (stringCompare(targetString, searchTerm) == 1)
      {
        return 1;
      }
    }
    ++targetString;
  }
  return 0;
}

Product *increaseCapacity(Product *productList, int *capacity)
{
  // Increase the product List capacity to double
  int newCapacity = 2 * (*capacity);
  if (*capacity == 0)
  {
    newCapacity = 1;
  }
  Product *newProductList = realloc(productList, newCapacity * sizeof(Product));
  if (newProductList == NULL)
  {
    printf("\tMemory allocation failed!\n");
    return NULL;
  }
  *capacity = newCapacity;
  return newProductList;
}

void addProduct(Product *productList, int *size)
{
  printf("\n__Add new Product__\n");

  int id = -1;
  char name[NAME_LENGTH] = "";
  float price = -1.0;
  int quantity = -1;

  printf("Enter product id: ");
  scanf("%d", &id);
  if (id < 1 || id > MAX_ID)
  {
    printf("Invalid product ID!\n");
    return;
  }
  while (getchar() != '\n')
    ;

  printf("Enter product name: ");
  fgets(name, sizeof(name), stdin);
  name[strcspn(name, "\n")] = '\0';
  if (strlen(name) == 0 || strlen(name) > NAME_LENGTH)
  {
    printf("Invalid product Name!\n");
    return;
  }

  printf("Enter product price: ");
  scanf("%f", &price);
  if (price < 0 || price > MAX_PRICE)
  {
    printf("Invalid price!\n");
    return;
  }

  printf("Enter product quantity: ");
  scanf("%d", &quantity);
  if (quantity < 0 || quantity > MAX_QUANTITY)
  {
    printf("Invalid product quantity!\n");
    return;
  }

  int foundIndex = findProductIndexByID(productList, id, *size);
  if (foundIndex >= 0)
  {
    printf("Duplicate Product ID! Product not added\n");
    return;
  }

  productList[*size].id = id;
  productList[*size].price = price;
  productList[*size].quantity = quantity;
  strncpy(productList[*size].name, name, NAME_LENGTH);

  ++*size;
}

void viewAllProducts(const Product *productsList, int size)
{
  printf("\n__All Products__\n");
  if (size == 0)
  {
    printf("No products in the list\n");
  }
  for (int productIndex = 0; productIndex < size; productIndex++)
  {
    printProduct(productsList[productIndex]);
    printf("\n");
  }
}

void handleUpdateQuantity(Product *productList, int size)
{
  printf("\n__Update Quantity__\n");
  int newQuantity = -1;
  int productID = -1;

  printf("Enter the product ID: ");
  scanf("%d", &productID);
  if (productID < 1 || productID > MAX_ID)
  {
    printf("Invalid product ID!\n");
    return;
  }

  printf("Enter new quantity: ");
  scanf("%d", &newQuantity);
  if (newQuantity < 0 || newQuantity > MAX_QUANTITY)
  {
    printf("Invalid quantity!\n");
    return;
  }

  int foundIndex = findProductIndexByID(productList, productID, size);
  if (foundIndex >= 0 && foundIndex < size)
  {
    productList[foundIndex].quantity = newQuantity;
    printf("Product quantity updated successfully!\n");
  }
  else
  {
    printf("Product not found!\n");
  }
}

void handleSearchByID(const Product *productList, int size)
{
  int id = -1;
  printf("\n__Search Product (By ID)__\n");
  printf("Enter the product ID: ");
  scanf("%d", &id);
  if (id < 1 || id > MAX_ID)
  {
    printf("Invalid ID entered!\n");
    return;
  }

  int index = findProductIndexByID(productList, id, size);

  if (index < size && index >= 0)
  {
    printf("product found:\n");
    printProduct(productList[index]);
  }
  else
  {
    printf("product not found!\n");
  }
}

void handleSearchByName(const Product *productList, int size)
{
  printf("\n__Search Product By N__\n");

  char name[NAME_LENGTH] = "";

  printf("Enter product Name: ");
  scanf(" %49[^\n]", name);
  if (strlen(name) > NAME_LENGTH || strlen(name) == 0)
  {
    printf("Invalid Name!");
  }

  int count = 0;
  for (int index = 0; index < size; index++)
  {
    if (containsIgnoreCase(productList[index].name, name) == 1)
    {
      printf("Product found: \n");
      printProduct(productList[index]);
      count++;
    }
  }
  if (count == 0)
  {
    printf("No product found!\n");
  }
}

void handleSearchByPriceRange(const Product *productList, int size)
{
  float lowerLimit = -1;
  float upperLimit = -1;
  printf("\n__Search Product (By price range)__\n");

  printf("Enter the lower price limit: ");
  scanf("%f", &lowerLimit);
  if (lowerLimit < 0 || lowerLimit > MAX_PRICE)
  {
    printf("Invalid price entered!\n");
    return;
  }

  printf("Enter the upper price limit: ");
  scanf("%f", &upperLimit);
  if (upperLimit < 0 || upperLimit > MAX_PRICE)
  {
    printf("Invalid price entered!\n");
    return;
  }

  int total = 0;

  for (int index = 0; index < size; index++)
  {
    if (productList[index].price >= lowerLimit && productList[index].price <= upperLimit)
    {
      printf("\n");
      printProduct(productList[index]);
      total++;
    }
  }
  if (total == 0)
  {
    printf("No product found!\n");
  }
}

void handleDeleteProduct(Product *productList, int *size)
{
  int id = -1;
  printf("\n__Delete Product__\n");
  printf("Enter the product ID to Delete: ");
  scanf("%d", &id);
  if (id < 1 || id > MAX_ID)
  {
    printf("Invalid Product ID!\n");
  }
  int index = findProductIndexByID(productList, id, *size);

  if (index < *size && index >= 0)
  {
    productList[index] = productList[*size - 1];
    --*size;

    // Cleanup
    productList[*size].id = 0;
    productList[*size].price = 0.0f;
    productList[*size].quantity = 0;
    productList[*size].name[0] = '\0';
    printf("product with id %d deleted!\n", id);
  }
  else
  {
    printf("product not found\n");
  }
}

void printMenu(void)
{
  printf("\n=====INVENTORY MENU=====\n");
  printf("1.Add New Product\n");
  printf("2.View All Products\n");
  printf("3.Update Quantity of a Product\n");
  printf("4.Search for a Product by ID\n");
  printf("5.Search for Products by Name (with partial match)\n");
  printf("6.Search for Products by Price Range\n");
  printf("7.Delete a Product by ID\n");
  printf("8.Exit the Program\n");
}

int main(void)
{
  int listSize = 0;

  int capacity = INITIAL_PRODUCTS;
  printf("Enter initial number of products (0-100): ");
  scanf("%d", &capacity);
  if (capacity < 1 || capacity > INITIAL_PRODUCTS)
  {
    printf("Enter a number between 1-100\n");
  }

  Product *productList = calloc(capacity, sizeof(Product));
  if (productList == NULL)
  {
    printf("Memory allocation failed\n");
    return -1;
  }

  while (listSize < capacity)
  {
    addProduct(productList, &listSize);
  }

  while (1)
  {
    printMenu();
    printf("\nEnter your choice: ");
    int choice;
    scanf("%d", &choice);

    switch (choice)
    {
    case 1:
      if (listSize == capacity)
      {
        Product *newList = increaseCapacity(productList, &capacity);
        if (newList != NULL)
        {
          productList = newList;
          addProduct(productList, &listSize);
        }
      }
      else
      {
        addProduct(productList, &listSize);
      }
      break;
    case 2:
      viewAllProducts(productList, listSize);
      break;
    case 3:
      handleUpdateQuantity(productList, listSize);
      break;
    case 4:
      handleSearchByID(productList, listSize);
      break;
    case 5:
      handleSearchByName(productList, listSize);
      break;
    case 6:
      handleSearchByPriceRange(productList, listSize);
      break;
    case 7:
      handleDeleteProduct(productList, &listSize);
      break;
    case 8:
      free(productList);
      return 0;
      break;

    default:
      printf("Invalid choice\n");
      break;
    }
  }

  return 0;
}